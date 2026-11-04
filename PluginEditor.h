#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DeliriumTVocalsAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DeliriumTVocalsAudioProcessorEditor(DeliriumTVocalsAudioProcessor&);
    ~DeliriumTVocalsAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    DeliriumTVocalsAudioProcessor& processor;
    juce::ComboBox presets;
    juce::Label title, subtitle, footer;
    juce::Slider input, output;
    juce::TextButton bypass { "MASTER BYPASS" };
    struct Module {
        juce::String name;
        juce::TextButton toggle;
        std::vector<std::unique_ptr<juce::Slider>> sliders;
        std::vector<std::unique_ptr<juce::Label>> labels;
        std::vector<std::unique_ptr<SliderAttachment>> attachments;
        std::unique_ptr<ButtonAttachment> buttonAttachment;
    };
    std::vector<std::unique_ptr<Module>> modules;
    std::unique_ptr<SliderAttachment> inputAttachment, outputAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    void addModule(const juce::String&, const juce::String&, std::initializer_list<std::pair<const char*, const char*>>);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeliriumTVocalsAudioProcessorEditor)
};
