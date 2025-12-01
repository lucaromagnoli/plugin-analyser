#pragma once

#include "Config.h"
#include "JuceHeader.h"

class ParameterConfigComponent : public juce::Component,
                                 public juce::ComboBox::Listener,
                                 public juce::TextEditor::Listener,
                                 public juce::Button::Listener {
public:
    ParameterConfigComponent(const juce::String& paramName, juce::AudioProcessorParameter* param = nullptr);
    ~ParameterConfigComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void buttonClicked(juce::Button* button) override;

    ParameterBucketConfig getConfig() const;
    void setConfig(const ParameterBucketConfig& config);

private:
    void updateUI();
    void detectSmartRange();

    juce::String paramName;
    juce::AudioProcessorParameter* pluginParam;

    juce::Label nameLabel;
    juce::Label strategyLabel;
    juce::ComboBox strategyCombo;
    juce::Label minLabel;
    juce::TextEditor minEditor;
    juce::Label maxLabel;
    juce::TextEditor maxEditor;
    juce::Label numBucketsLabel;
    juce::TextEditor numBucketsEditor;
    juce::Label valuesLabel;
    juce::TextEditor valuesEditor;
    juce::Label bucketCountLabel;
    juce::TextButton smartRangeButton;

    ParameterBucketConfig currentConfig;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterConfigComponent)
};
