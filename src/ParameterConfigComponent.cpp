#include "ParameterConfigComponent.h"
#include <algorithm>
#include <map>
#include <set>

ParameterConfigComponent::ParameterConfigComponent(const juce::String& paramName, juce::AudioProcessorParameter* param)
    : paramName(paramName), pluginParam(param) {
    nameLabel.setText(paramName, juce::dontSendNotification);
    nameLabel.setFont(juce::Font(16.0f).boldened());
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

    bucketCountLabel.setText("Buckets: 2", juce::dontSendNotification);
    bucketCountLabel.setFont(juce::Font(12.0f).italicised());
    addAndMakeVisible(bucketCountLabel);

    smartRangeButton.setButtonText("Auto-Detect Range");
    smartRangeButton.addListener(this);
    addAndMakeVisible(smartRangeButton);

    currentConfig.paramName = paramName;
    currentConfig.strategy = "Linear";
    currentConfig.min = 0.0f;
    currentConfig.max = 1.0f;
    currentConfig.numBuckets = 5;
    currentConfig.values = {0.0f, 1.0f};

    // Auto-detect smart range if parameter is available
    if (pluginParam != nullptr) {
        detectSmartRange();
    }

    updateUI();
}

ParameterConfigComponent::~ParameterConfigComponent() {}

void ParameterConfigComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
}

void ParameterConfigComponent::resized() {
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
    bounds.removeFromTop(5);
    auto buttonRow = bounds.removeFromTop(rowHeight);
    bucketCountLabel.setBounds(buttonRow.removeFromLeft(120));
    buttonRow.removeFromLeft(10);
    smartRangeButton.setBounds(buttonRow.removeFromLeft(150));
}

void ParameterConfigComponent::comboBoxChanged(juce::ComboBox* comboBox) {
    if (comboBox == &strategyCombo) {
        updateUI();
    }
}

void ParameterConfigComponent::buttonClicked(juce::Button* button) {
    if (button == &smartRangeButton) {
        detectSmartRange();
        updateUI();
    }
}

void ParameterConfigComponent::textEditorTextChanged(juce::TextEditor& editor) {
    // Update config when text changes
    if (&editor == &minEditor) {
        currentConfig.min = (float)editor.getText().getDoubleValue();
    } else if (&editor == &maxEditor) {
        currentConfig.max = (float)editor.getText().getDoubleValue();
    } else if (&editor == &numBucketsEditor) {
        currentConfig.numBuckets = editor.getText().getIntValue();
    } else if (&editor == &valuesEditor) {
        juce::StringArray tokens;
        tokens.addTokens(editor.getText(), ",", "");
        currentConfig.values.clear();
        for (const auto& token : tokens) {
            float val = (float)token.trim().getDoubleValue();
            currentConfig.values.push_back(val);
        }
        // Update bucket count label for ExplicitValues
        if (currentConfig.strategy == "ExplicitValues") {
            bucketCountLabel.setText("Buckets: " + juce::String(currentConfig.values.size()),
                                     juce::dontSendNotification);
        }
    }
}

void ParameterConfigComponent::updateUI() {
    int strategyId = strategyCombo.getSelectedId();

    bool showMinMax = (strategyId == 1 || strategyId == 3 || strategyId == 4); // Linear, Log, EdgeAndCenter
    bool showBuckets = (strategyId == 1 || strategyId == 3);                   // Linear, Log
    bool showValues = (strategyId == 2);                                       // ExplicitValues

    minLabel.setVisible(showMinMax);
    minEditor.setVisible(showMinMax);
    maxLabel.setVisible(showMinMax);
    maxEditor.setVisible(showMinMax);
    numBucketsLabel.setVisible(showBuckets);
    numBucketsEditor.setVisible(showBuckets);
    valuesLabel.setVisible(showValues);
    valuesEditor.setVisible(showValues);
    bucketCountLabel.setVisible(showValues); // Show bucket count for ExplicitValues

    // Update bucket count when ExplicitValues is selected
    if (showValues) {
        bucketCountLabel.setText("Buckets: " + juce::String(currentConfig.values.size()), juce::dontSendNotification);
    }

    // Update strategy string
    if (strategyId == 1)
        currentConfig.strategy = "Linear";
    else if (strategyId == 2)
        currentConfig.strategy = "ExplicitValues";
    else if (strategyId == 3)
        currentConfig.strategy = "Log";
    else if (strategyId == 4)
        currentConfig.strategy = "EdgeAndCenter";

    resized();
}

ParameterBucketConfig ParameterConfigComponent::getConfig() const {
    return currentConfig;
}

void ParameterConfigComponent::setConfig(const ParameterBucketConfig& config) {
    currentConfig = config;

    if (config.strategy == "Linear")
        strategyCombo.setSelectedId(1);
    else if (config.strategy == "ExplicitValues")
        strategyCombo.setSelectedId(2);
    else if (config.strategy == "Log")
        strategyCombo.setSelectedId(3);
    else if (config.strategy == "EdgeAndCenter")
        strategyCombo.setSelectedId(4);

    minEditor.setText(juce::String(config.min), juce::dontSendNotification);
    maxEditor.setText(juce::String(config.max), juce::dontSendNotification);
    numBucketsEditor.setText(juce::String(config.numBuckets), juce::dontSendNotification);

    juce::String valuesStr;
    for (size_t i = 0; i < config.values.size(); ++i) {
        if (i > 0)
            valuesStr += ", ";
        valuesStr += juce::String(config.values[i]);
    }
    valuesEditor.setText(valuesStr, juce::dontSendNotification);

    updateUI();
}

void ParameterConfigComponent::detectSmartRange() {
    if (pluginParam == nullptr) {
        std::cerr << "[detectSmartRange] No parameter provided for: " << paramName << std::endl;
        return;
    }

    std::cerr << "[detectSmartRange] Detecting range for: " << paramName << std::endl;

    try {
        // Check if it's a choice parameter (discrete values)
        if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(pluginParam)) {
            std::cerr << "[detectSmartRange] Found Choice parameter" << std::endl;
            std::cerr << "[detectSmartRange] All choices from API:" << std::endl;
            for (int i = 0; i < choiceParam->choices.size(); ++i) {
                float normValue = (float)i / (float)(choiceParam->choices.size() - 1);
                juce::String text = choiceParam->choices[i];
                std::cerr << "[detectSmartRange]   index=" << i << " normVal=" << normValue << " text='" << text << "'"
                          << std::endl;
            }

        } else if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(pluginParam)) {
            std::cerr << "[detectSmartRange] Found Float parameter" << std::endl;
            // Check if this looks like a gain parameter (input/output/pre) with range around -24 to +24
            float minVal = floatParam->range.start;
            float maxVal = floatParam->range.end;

            std::cerr << "[detectSmartRange] Range: " << minVal << " to " << maxVal << std::endl;

            // If range is approximately -24 to +24 (or similar symmetric gain range), use ExplicitValues
            if ((minVal <= -20.0f && minVal >= -30.0f) && (maxVal >= 20.0f && maxVal <= 30.0f)) {
                std::cerr << "[detectSmartRange] Detected gain parameter, using ExplicitValues" << std::endl;
                // Use ExplicitValues with min, center, max (3 buckets)
                currentConfig.strategy = "ExplicitValues";
                currentConfig.values = {minVal, 0.0f, maxVal};

                // Update UI
                strategyCombo.setSelectedId(2); // ExplicitValues
                juce::String valuesStr = juce::String(minVal, 1) + ", 0.0, " + juce::String(maxVal, 1);
                valuesEditor.setText(valuesStr, juce::dontSendNotification);
                bucketCountLabel.setText("Buckets: 3", juce::dontSendNotification);
            } else {
                // Use Linear strategy with actual parameter range
                std::cerr << "[detectSmartRange] Using Linear with range " << minVal << " to " << maxVal << std::endl;
                currentConfig.strategy = "Linear";
                currentConfig.min = minVal;
                currentConfig.max = maxVal;
                currentConfig.numBuckets = 3; // Default to 3 buckets

                // Update UI
                strategyCombo.setSelectedId(1); // Linear
                minEditor.setText(juce::String(currentConfig.min, 2), juce::dontSendNotification);
                maxEditor.setText(juce::String(currentConfig.max, 2), juce::dontSendNotification);
                numBucketsEditor.setText("3", juce::dontSendNotification);
            }

        } else if (auto* intParam = dynamic_cast<juce::AudioParameterInt*>(pluginParam)) {
            // Use ExplicitValues strategy - sample normalized values
            // Since we can't access the range directly, use common sampling points
            currentConfig.strategy = "ExplicitValues";
            currentConfig.values = {0.0f, 0.5f, 1.0f}; // min, mid, max normalized

            // Update UI
            strategyCombo.setSelectedId(2); // ExplicitValues
            valuesEditor.setText("0.0, 0.5, 1.0", juce::dontSendNotification);
            bucketCountLabel.setText("Buckets: 3", juce::dontSendNotification);

        } else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(pluginParam)) {
            // Use ExplicitValues with 0.0 and 1.0
            currentConfig.strategy = "ExplicitValues";
            currentConfig.values = {0.0f, 1.0f};

            // Update UI
            strategyCombo.setSelectedId(2); // ExplicitValues
            valuesEditor.setText("0.0, 1.0", juce::dontSendNotification);
            bucketCountLabel.setText("Buckets: 2", juce::dontSendNotification);

        } else {
            // Generic parameter (VST3Parameter) - Detect discrete ranges and print text values
            std::cerr << "[detectSmartRange] Generic parameter, detecting discrete ranges..." << std::endl;

            // Store original value
            float originalValue = pluginParam->getValue();

            // Sample to find where text changes (discrete boundaries)
            juce::String prevText;
            float prevPos = 0.0f;
            std::vector<std::pair<float, float>> ranges; // (start, end) pairs
            std::vector<juce::String> rangeTexts;

            for (float normalizedPos = 0.0f; normalizedPos <= 1.0f; normalizedPos += 0.001f) {
                pluginParam->setValueNotifyingHost(normalizedPos);
                juce::String text = pluginParam->getText(pluginParam->getValue(), 512);

                if (normalizedPos == 0.0f) {
                    prevText = text;
                    prevPos = 0.0f;
                } else if (text != prevText) {
                    // Text changed - this is a boundary
                    ranges.push_back({prevPos, normalizedPos});
                    rangeTexts.push_back(prevText);
                    prevText = text;
                    prevPos = normalizedPos;
                }
            }

            // Add the last range
            ranges.push_back({prevPos, 1.0f});
            rangeTexts.push_back(prevText);

            // Restore original value
            pluginParam->setValueNotifyingHost(originalValue);

            // Print all ranges and their text values
            std::cerr << "[detectSmartRange] Found " << ranges.size() << " discrete ranges:" << std::endl;
            for (size_t i = 0; i < ranges.size(); ++i) {
                std::cerr << "[detectSmartRange]   Range [" << ranges[i].first << " - " << ranges[i].second
                          << "] -> text='" << rangeTexts[i] << "'" << std::endl;
            }

            // Don't do anything smart - just leave it at defaults
            currentConfig.strategy = "Linear";
            currentConfig.min = 0.0f;
            currentConfig.max = 1.0f;
            currentConfig.numBuckets = 3;

            strategyCombo.setSelectedId(1); // Linear
            minEditor.setText("0.0", juce::dontSendNotification);
            maxEditor.setText("1.0", juce::dontSendNotification);
            numBucketsEditor.setText("3", juce::dontSendNotification);
        }
    } catch (const std::exception& e) {
        std::cerr << "[detectSmartRange] ERROR: Exception caught: " << e.what() << std::endl;
        // Fallback to safe defaults
        currentConfig.strategy = "Linear";
        currentConfig.min = 0.0f;
        currentConfig.max = 1.0f;
        currentConfig.numBuckets = 3;
        strategyCombo.setSelectedId(1);
        minEditor.setText("0.0", juce::dontSendNotification);
        maxEditor.setText("1.0", juce::dontSendNotification);
        numBucketsEditor.setText("3", juce::dontSendNotification);
    } catch (...) {
        std::cerr << "[detectSmartRange] ERROR: Unknown exception caught" << std::endl;
        // Fallback to safe defaults
        currentConfig.strategy = "Linear";
        currentConfig.min = 0.0f;
        currentConfig.max = 1.0f;
        currentConfig.numBuckets = 3;
        strategyCombo.setSelectedId(1);
        minEditor.setText("0.0", juce::dontSendNotification);
        maxEditor.setText("1.0", juce::dontSendNotification);
        numBucketsEditor.setText("3", juce::dontSendNotification);
    }
}
