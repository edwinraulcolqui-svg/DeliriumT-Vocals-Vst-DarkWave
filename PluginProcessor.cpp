#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout()
{
    using APF = juce::AudioParameterFloat;
    using APB = juce::AudioParameterBool;
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    auto addF = [&p](const char* id, const char* name, float lo, float hi, float def, const char* unit = "")
    {
        p.add(std::make_unique<APF>(id, name, juce::NormalisableRange<float>(lo, hi, 0.01f), def, unit));
    };
    auto addB = [&p](const char* id, const char* name, bool def)
    {
        p.add(std::make_unique<APB>(id, name, def));
    };

    addB("masterBypass", "Master Bypass", false);
    addF("input", "Input", -24, 18, 0, " dB");
    addF("output", "Output", -24, 18, 0, " dB");
    addB("eqOn", "EQ", true); addF("low", "Low", -12, 12, 0, " dB");
    addF("mid", "Mid", -12, 12, -1, " dB"); addF("high", "High", -12, 12, -2, " dB");
    addF("midFreq", "Mid Freq", 200, 6000, 1400, " Hz");
    addB("compOn", "Compressor", true); addF("threshold", "Threshold", -48, 0, -18, " dB");
    addF("ratio", "Ratio", 1, 12, 3, ":1"); addF("attack", "Attack", 1, 100, 12, " ms");
    addF("release", "Release", 20, 500, 120, " ms"); addF("makeup", "Makeup", 0, 18, 3, " dB");
    addB("chorusOn", "Chorus", false); addF("chorusRate", "Rate", 0.05f, 5, 0.35f, " Hz");
    addF("chorusDepth", "Depth", 0, 1, 0.35f); addF("chorusMix", "Mix", 0, 1, 0.22f);
    addB("delayOn", "Delay", false); addF("delayTime", "Time", 40, 1000, 380, " ms");
    addF("feedback", "Feedback", 0, 0.9f, 0.28f); addF("delayMix", "Mix", 0, 1, 0.18f);
    addF("stereo", "Stereo", 0, 1, 0.6f);
    addB("reverbOn", "Reverb", true); addF("decay", "Decay", 0.2f, 10, 3.2f, " s");
    addF("room", "Size", 0, 1, 0.75f); addF("damping", "Damping", 0, 1, 0.55f);
    addF("predelay", "Pre-delay", 0, 120, 24, " ms"); addF("reverbMix", "Mix", 0, 1, 0.24f);
    addB("satOn", "Saturation", false); addF("drive", "Drive", 0, 18, 4, " dB");
    addF("tone", "Tone", 0, 1, 0.55f); addF("satMix", "Mix", 0, 1, 0.25f);
    return p;
}

DeliriumTVocalsAudioProcessor::DeliriumTVocalsAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
  parameters(*this, nullptr, "PARAMETERS", makeLayout())
{}

void DeliriumTVocalsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    delayBuffer.setSize(2, (int)(sr * 2.1) + samplesPerBlock + 8);
    chorusBuffer.setSize(2, (int)(sr * 0.05) + samplesPerBlock + 8);
    delayBuffer.clear(); chorusBuffer.clear(); delayWrite = chorusWrite = 0;
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = 0.75f; rp.damping = 0.55f; rp.wetLevel = 0.24f; rp.dryLevel = 0.76f; rp.width = 1.0f; rp.freezeMode = 0.0f;
    reverb.setParameters(rp); reverb.reset();
    for (auto& f : lowFilters) f.reset();
    for (auto& f : highFilters) f.reset();
    compEnvelope[0] = compEnvelope[1] = 0.0f;
}

void DeliriumTVocalsAudioProcessor::releaseResources()
{
    delayBuffer.setSize(0, 0); chorusBuffer.setSize(0, 0);
}

void DeliriumTVocalsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    if (parameters.getRawParameterValue("masterBypass")->load() > 0.5f) return;
    const int n = buffer.getNumSamples();
    auto v = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };
    const float inGain = juce::Decibels::decibelsToGain(v("input"));
    const float outGain = juce::Decibels::decibelsToGain(v("output"));
    const bool eq = v("eqOn") > .5f, comp = v("compOn") > .5f, chorus = v("chorusOn") > .5f;
    const bool delay = v("delayOn") > .5f, rev = v("reverbOn") > .5f, sat = v("satOn") > .5f;

    const float lowG = juce::Decibels::decibelsToGain(v("low"));
    const float midG = juce::Decibels::decibelsToGain(v("mid"));
    const float highG = juce::Decibels::decibelsToGain(v("high"));
    const float threshold = juce::Decibels::decibelsToGain(v("threshold"));
    const float ratio = v("ratio"), attack = std::exp(-1.0f / (float)(sr * v("attack") * .001f));
    const float release = std::exp(-1.0f / (float)(sr * v("release") * .001f));
    const int dSamp = juce::jlimit(1, delayBuffer.getNumSamples()-1, (int)(sr * v("delayTime") * .001f));
    const int cBase = juce::jlimit(1, chorusBuffer.getNumSamples()-2, (int)(sr * .012f));
    const float feedback = v("feedback"), dMix = v("delayMix"), cMix = v("chorusMix");
    const float depth = v("chorusDepth"), rate = v("chorusRate");
    const float drive = juce::Decibels::decibelsToGain(v("drive"));
    const float satMix = v("satMix"), tone = v("tone");
    const float preGain = inGain;

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = v("room"); rp.damping = v("damping"); rp.wetLevel = v("reverbMix");
    rp.dryLevel = 1.0f - v("reverbMix"); rp.width = 1.0f; rp.freezeMode = 0.0f;
    reverb.setParameters(rp);

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* x = buffer.getWritePointer(ch);
        for (int i = 0; i < n; ++i)
        {
            float s = x[i] * preGain;
            if (eq)
            {
                const float lowCut = 180.0f, highCut = v("midFreq");
                const float aL = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * lowCut / (float)sr);
                const float aH = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * highCut / (float)sr);
                static thread_local float lp[2] = {0,0}, hp[2] = {0,0};
                lp[ch] += aL * (s - lp[ch]); hp[ch] += aH * (s - hp[ch]);
                const float low = lp[ch], high = s - hp[ch], midBand = s - low - high;
                s = low * lowG + midBand * midG + high * highG;
            }
            if (comp)
            {
                const float level = std::abs(s);
                compEnvelope[ch] = (level > compEnvelope[ch] ? attack : release) * compEnvelope[ch]
                                 + (1.0f - (level > compEnvelope[ch] ? attack : release)) * level;
                if (compEnvelope[ch] > threshold)
                {
                    const float compressed = threshold + (compEnvelope[ch] - threshold) / ratio;
                    s *= compressed / juce::jmax(compEnvelope[ch], 0.00001f);
                }
                s *= juce::Decibels::decibelsToGain(v("makeup"));
            }
            if (sat)
            {
                const float shaped = std::tanh(s * drive * (0.65f + tone));
                s = s * (1.0f - satMix) + shaped * satMix;
            }
            if (chorus)
            {
                const float phase = (float)(2.0 * juce::MathConstants<double>::pi * rate * (double)(chorusWrite + i) / sr);
                const float mod = (float)std::sin(phase) * depth * sr * 0.004f;
                const int read = (chorusWrite - cBase - (int)mod + chorusBuffer.getNumSamples()) % chorusBuffer.getNumSamples();
                const float delayed = chorusBuffer.getSample(ch, read);
                chorusBuffer.setSample(ch, chorusWrite, s);
                s = s * (1.0f - cMix) + delayed * cMix;
            }
            if (delay)
            {
                const int channelOffset = (int)(v("stereo") * (float)sr * 0.012f * (float)ch);
                const int read = (delayWrite - dSamp - channelOffset + delayBuffer.getNumSamples() * 2) % delayBuffer.getNumSamples();
                const float echo = delayBuffer.getSample(ch, read);
                delayBuffer.setSample(ch, delayWrite, s + echo * feedback);
                s = s * (1.0f - dMix) + echo * dMix;
            }
            x[i] = s * outGain;
        }
    }
    if (channels == 2)
    {
        if (chorus) chorusWrite = (chorusWrite + n) % chorusBuffer.getNumSamples();
        if (delay) delayWrite = (delayWrite + n) % delayBuffer.getNumSamples();
        if (rev) reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), n);
    }
    else if (channels == 1 && rev)
        reverb.processMono(buffer.getWritePointer(0), n);
}

void DeliriumTVocalsAudioProcessor::loadPreset(int index)
{
    auto set = [this](const char* id, float value) {
        if (auto* p = parameters.getParameter(id)) p->setValueNotifyingHost(p->convertTo0to1(value));
    };
    // Each preset is a complete starting point; all controls remain editable.
    set("eqOn", 1); set("compOn", 1); set("reverbOn", 1);
    if (index == 0) { set("low",-1); set("mid",1); set("high",-2); set("threshold",-20); set("ratio",3); set("chorusOn",0); set("delayOn",0); set("reverbMix",.22f); set("decay",2.8f); set("satOn",0); }
    if (index == 1) { set("low",-3); set("mid",0); set("high",-4); set("threshold",-24); set("ratio",4); set("chorusOn",1); set("chorusMix",.32f); set("delayOn",1); set("delayMix",.16f); set("reverbMix",.28f); set("decay",4.2f); }
    if (index == 2) { set("low",-2); set("mid",-2); set("high",-5); set("threshold",-22); set("ratio",2.5f); set("chorusOn",1); set("chorusMix",.4f); set("delayOn",1); set("delayMix",.25f); set("reverbMix",.42f); set("decay",7.5f); }
    if (index == 3) { set("low",1); set("mid",2); set("high",-3); set("threshold",-16); set("ratio",5); set("chorusOn",0); set("delayOn",0); set("satOn",1); set("drive",7); set("satMix",.35f); set("reverbMix",.16f); }
    if (index == 4) { set("low",-2); set("mid",1); set("high",-2); set("threshold",-18); set("ratio",3); set("chorusOn",1); set("chorusMix",.2f); set("delayOn",1); set("delayMix",.12f); set("reverbMix",.2f); set("decay",2.2f); }
}

void DeliriumTVocalsAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void DeliriumTVocalsAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DeliriumTVocalsAudioProcessor();
}
