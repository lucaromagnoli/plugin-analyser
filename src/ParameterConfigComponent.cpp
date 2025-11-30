#include "ParameterConfigComponent.h"

ParameterConfigComponent::ParameterConfigComponent(const juce::String& paramName)
    : paramName(paramName)
{
    nameLabel.setText(paramName, juce::dontSendNotification);
    nameLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(nameLabel);
    
    strategyLabel.setText("Strategy:", juce::dontSendNotification);
    addAndMakeVisible(strategyLabel);
    
    strategyCombo.addItem("Linear", 1);
    strategyCombo.addItem("ExplicitValues", 2);
    strategyCombo.addItem("Log", 3);
    strategyCombo.addItem("EdgeAndCenter", 4);
    strategyCombo.setSelectedId(1);
    strategyCombo.addListener(this);
    addAndMakeVisible(strategyCombo);
    
    minLabel.setText("Min:", juce::dontSendNotification);
    addAndMakeVisible(minLabel);
    minEditor.setText("0.0", juce::dontSendNotification);
    minEditor.addListener(this);
    addAndMakeVisible(minEditor);
    
    maxLabel.setText("Max:", juce::dontSendNotification);
    addAndMakeVisible(maxLabel);
    maxEditor.setText("1.0", juce::dontSendNotification);
    maxEditor.addListener(this);
    addAndMakeVisible(maxEditor);
    
    numBucketsLabel.setText("Buckets:", juce::dontSendNotification);
    addAndMakeVisible(numBucketsLabel);
    numBucketsEditor.setText("5", juce::dontSendNotification);
    numBucketsEditor.addListener(this);
    addAndMakeVisible(numBucketsEditor);
    
    valuesLabel.setText("Values (comma-separated):", juce::dontSendNotification);
    addAndMakeVisible(valuesLabel);
    valuesEditor.setText("0.0, 1.0", juce::dontSendNotification);
    valuesEditor.addListener(this);
    addAndMakeVisible(valuesEditor);
    
    currentConfig.paramName = paramName;
    currentConfig.strategy = "Linear";
    currentConfig.min = 0.0f;
    currentConfig.max = 1.0f;
    currentConfig.numBuckets = 5;
    currentConfig.values = {0.0f, 1.0f};
    
    updateUI();
}

ParameterConfigComponent::~ParameterConfigComponent()
{
}

void ParameterConfigComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
}

void ParameterConfigComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    int rowHeight = 30;
    int y = 10;
    
    nameLabel.setBounds(bounds.removeFromTop(rowHeight));
    bounds.removeFromTop(5);
    
    auto strategyRow = bounds.removeFromTop(rowHeight);
    strategyLabel.setBounds(strategyRow.removeFromLeft(100));
    strategyCombo.setBounds(strategyRow.removeFromLeft(150));
    bounds.removeFromTop(5);
    
    auto minMaxRow = bounds.removeFromTop(rowHeight);
    minLabel.setBounds(minMaxRow.removeFromLeft(50));
    minEditor.setBounds(minMaxRow.removeFromLeft(80));
    minMaxRow.removeFromLeft(20);
    maxLabel.setBounds(minMaxRow.removeFromLeft(50));
    maxEditor.setBounds(minMaxRow.removeFromLeft(80));
    bounds.removeFromTop(5);
    
    auto bucketsRow = bounds.removeFromTop(rowHeight);
    numBucketsLabel.setBounds(bucketsRow.removeFromLeft(100));
    numBucketsEditor.setBounds(bucketsRow.removeFromLeft(80));
    bounds.removeFromTop(5);
    
    valuesLabel.setBounds(bounds.removeFromTop(rowHeight));
    valuesEditor.setBounds(bounds.removeFromTop(40));
}

void ParameterConfigComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &strategyCombo)
    {
        updateUI();
    }
}

void ParameterConfigComponent::textEditorTextChanged(juce::TextEditor& editor)
{
    // Update config when text changes
    if (&editor == &minEditor)
    {
        currentConfig.min = (float)editor.getText().getDoubleValue();
    }
    else if (&editor == &maxEditor)
    {
        currentConfig.max = (float)editor.getText().getDoubleValue();
    }
    else if (&editor == &numBucketsEditor)
    {
        currentConfig.numBuckets = editor.getText().getIntValue();
    }
    else if (&editor == &valuesEditor)
    {
        juce::StringArray tokens;
        tokens.addTokens(editor.getText(), ",", "");
        currentConfig.values.clear();
        for (const auto& token : tokens)
        {
            float val = (float)token.trim().getDoubleValue();
            currentConfig.values.push_back(val);
        }
    }
}

void ParameterConfigComponent::updateUI()
{
    int strategyId = strategyCombo.getSelectedId();
    
    bool showMinMax = (strategyId == 1 || strategyId == 3 || strategyId == 4); // Linear, Log, EdgeAndCenter
    bool showBuckets = (strategyId == 1 || strategyId == 3); // Linear, Log
    bool showValues = (strategyId == 2); // ExplicitValues
    
    minLabel.setVisible(showMinMax);
    minEditor.setVisible(showMinMax);
    maxLabel.setVisible(showMinMax);
    maxEditor.setVisible(showMinMax);
    numBucketsLabel.setVisible(showBuckets);
    numBucketsEditor.setVisible(showBuckets);
    valuesLabel.setVisible(showValues);
    valuesEditor.setVisible(showValues);
    
    // Update strategy string
    if (strategyId == 1) currentConfig.strategy = "Linear";
    else if (strategyId == 2) currentConfig.strategy = "ExplicitValues";
    else if (strategyId == 3) currentConfig.strategy = "Log";
    else if (strategyId == 4) currentConfig.strategy = "EdgeAndCenter";
    
    resized();
}

ParameterBucketConfig ParameterConfigComponent::getConfig() const
{
    return currentConfig;
}

void ParameterConfigComponent::setConfig(const ParameterBucketConfig& config)
{
    currentConfig = config;
    
    if (config.strategy == "Linear") strategyCombo.setSelectedId(1);
    else if (config.strategy == "ExplicitValues") strategyCombo.setSelectedId(2);
    else if (config.strategy == "Log") strategyCombo.setSelectedId(3);
    else if (config.strategy == "EdgeAndCenter") strategyCombo.setSelectedId(4);
    
    minEditor.setText(juce::String(config.min), juce::dontSendNotification);
    maxEditor.setText(juce::String(config.max), juce::dontSendNotification);
    numBucketsEditor.setText(juce::String(config.numBuckets), juce::dontSendNotification);
    
    juce::String valuesStr;
    for (size_t i = 0; i < config.values.size(); ++i)
    {
        if (i > 0) valuesStr += ", ";
        valuesStr += juce::String(config.values[i]);
    }
    valuesEditor.setText(valuesStr, juce::dontSendNotification);
    
    updateUI();
}

