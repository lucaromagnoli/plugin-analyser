#pragma once

#include "Config.h"
#include "MeasurementEngine.h"
#include "PluginLoader.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>

class ParameterConfigComponent;
class MeasurementConfigComponent;

class MainComponent : public juce::Component,
                      public juce::Button::Listener,
                      public juce::TextEditor::Listener,
                      public juce::ListBoxModel {
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;

    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;

private:
    void loadPlugin();
    void scanPluginParameters();
    void updateParameterList();
    void runMeasurement();
    Config buildConfigFromUI();
    void showError(const juce::String& message);

    // Plugin selection
    juce::Label pluginPathLabel;
    juce::TextEditor pluginPathEditor;
    juce::TextButton browseButton;
    juce::TextButton loadPluginButton;

    // Plugin info
    juce::Label pluginInfoLabel;

    // Parameter selection and configuration
    juce::Label parametersLabel;
    juce::ListBox parameterListBox;
    std::vector<juce::String> availableParameters;
    std::vector<bool> selectedParameters;
    juce::TextButton selectAllButton;
    juce::TextButton deselectAllButton;

    // Parameter configuration panel
    juce::Viewport parameterConfigViewport;
    juce::Component parameterConfigContainer;
    std::vector<std::unique_ptr<ParameterConfigComponent>> parameterConfigComponents;

    // Measurement configuration
    std::unique_ptr<MeasurementConfigComponent> measurementConfig;

    // Output
    juce::Label outputPathLabel;
    juce::TextEditor outputPathEditor;
    juce::TextButton browseOutputButton;

    // Run button
    juce::TextButton runMeasurementButton;

    // Progress
    juce::Label progressLabel;
    juce::ProgressBar progressBar;
    double progress = 0.0;

    // Plugin instance
    std::unique_ptr<juce::AudioPluginInstance> pluginInstance;
    std::map<juce::String, juce::AudioProcessorParameter*> parameterMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
