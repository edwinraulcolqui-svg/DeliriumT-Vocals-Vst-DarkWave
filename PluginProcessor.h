#pragma once
#include <JuceHeader.h>

class DeliriumTVocalsAudioProcessor : public juce::AudioProcessor
{
public:
    DeliriumTVocalsAudioProcessor();
    ~DeliriumTVocalsAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "DeliriumT Vocals"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    void loadPreset(int presetIndex);

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> delayBuffer, chorusBuffer;
    int delayWrite = 0, chorusWrite = 0;
    double sr = 44100.0;
    float compEnvelope[2] { 0.0f, 0.0f };
    juce::IIRFilter lowFilters[2], highFilters[2];
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeliriumTVocalsAudioProcessor)
};
