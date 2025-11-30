#pragma once

#include "Config.h"
#include <JuceHeader.h>

class MeasurementConfigComponent : public juce::Component,
                                   public juce::ComboBox::Listener,
                                   public juce::TextEditor::Listener,
                                   public juce::Button::Listener {
public:
    MeasurementConfigComponent();
    ~MeasurementConfigComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void buttonClicked(juce::Button* button) override;

    void fillConfig(Config& config);
    void loadFromConfig(const Config& config);

private:
    void updateUI();

    // Signal type
    juce::Label signalTypeLabel;
    juce::ComboBox signalTypeCombo;

    // Sine settings
    juce::Label sineFreqLabel;
    juce::TextEditor sineFreqEditor;

    // Sweep settings
    juce::Label sweepStartLabel;
    juce::TextEditor sweepStartEditor;
    juce::Label sweepEndLabel;
    juce::TextEditor sweepEndEditor;

    // Audio settings
    juce::Label sampleRateLabel;
    juce::TextEditor sampleRateEditor;
    juce::Label secondsLabel;
    juce::TextEditor secondsEditor;
    juce::Label blockSizeLabel;
    juce::TextEditor blockSizeEditor;

    // Input gain buckets
    juce::Label inputGainLabel;
    juce::TextEditor inputGainEditor;

    // Analyzers
    juce::Label analyzersLabel;
    juce::ToggleButton rawCsvButton;
    juce::ToggleButton rmsPeakButton;
    juce::ToggleButton transferCurveButton;
    juce::ToggleButton linearResponseButton;
    juce::ToggleButton thdButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeasurementConfigComponent)
};
