#include "MainComponent.h"
#include "MeasurementConfigComponent.h"
#include "ParameterConfigComponent.h"
#include <thread>

MainComponent::MainComponent()
    : pluginPathLabel("Plugin Path:", "Plugin Path:"), pluginInfoLabel("", "No plugin loaded"),
      parametersLabel("Parameters:", "Parameters:"), outputPathLabel("Output Path:", "Output Path:"),
      progressLabel("", "Ready"), progressBar(progress) {
    // Plugin path section
    addAndMakeVisible(pluginPathLabel);
    pluginPathEditor.setText("/absolute/path/to/plugin.vst3", juce::dontSendNotification);
    pluginPathEditor.addListener(this);
    addAndMakeVisible(pluginPathEditor);

    browseButton.setButtonText("Browse...");
    browseButton.addListener(this);
    addAndMakeVisible(browseButton);

    loadPluginButton.setButtonText("Load Plugin");
    loadPluginButton.addListener(this);
    addAndMakeVisible(loadPluginButton);

    addAndMakeVisible(pluginInfoLabel);

    // Parameter list
    addAndMakeVisible(parametersLabel);
    parameterListBox.setModel(this);
    addAndMakeVisible(parameterListBox);

    selectAllButton.setButtonText("Select All");
    selectAllButton.addListener(this);
    addAndMakeVisible(selectAllButton);

    deselectAllButton.setButtonText("Deselect All");
    deselectAllButton.addListener(this);
    addAndMakeVisible(deselectAllButton);

    // Parameter config viewport
    parameterConfigViewport.setViewedComponent(&parameterConfigContainer, false);
    addAndMakeVisible(parameterConfigViewport);

    // Measurement config
    measurementConfig = std::make_unique<MeasurementConfigComponent>();
    addAndMakeVisible(measurementConfig.get());

    // Output path
    addAndMakeVisible(outputPathLabel);
    outputPathEditor.setText(juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                                 .getChildFile("plugin_measure_output")
                                 .getFullPathName(),
                             juce::dontSendNotification);
    outputPathEditor.addListener(this);
    addAndMakeVisible(outputPathEditor);

    browseOutputButton.setButtonText("Browse...");
    browseOutputButton.addListener(this);
    addAndMakeVisible(browseOutputButton);

    // Run button
    runMeasurementButton.setButtonText("Run Measurement");
    runMeasurementButton.addListener(this);
    runMeasurementButton.setEnabled(false);
    addAndMakeVisible(runMeasurementButton);

    // Progress
    addAndMakeVisible(progressLabel);
    addAndMakeVisible(progressBar);
    progressBar.setPercentageDisplay(false);

    setSize(1200, 800);
}

MainComponent::~MainComponent() {}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    auto bounds = getLocalBounds().reduced(10);

    // Plugin path section (top)
    auto pluginSection = bounds.removeFromTop(80);
    pluginPathLabel.setBounds(pluginSection.removeFromTop(25));
    auto pluginRow = pluginSection.removeFromTop(30);
    pluginPathEditor.setBounds(pluginRow.removeFromLeft(600));
    pluginRow.removeFromLeft(10);
    browseButton.setBounds(pluginRow.removeFromLeft(100));
    pluginRow.removeFromLeft(10);
    loadPluginButton.setBounds(pluginRow.removeFromLeft(120));
    pluginInfoLabel.setBounds(pluginSection);
    bounds.removeFromTop(10);

    // Main content area (side by side)
    auto contentArea = bounds.removeFromTop(500);

    // Left: Parameter list and config
    auto leftPanel = contentArea.removeFromLeft(500);
    parametersLabel.setBounds(leftPanel.removeFromTop(25));
    auto buttonRow = leftPanel.removeFromTop(30);
    selectAllButton.setBounds(buttonRow.removeFromLeft(100));
    buttonRow.removeFromLeft(10);
    deselectAllButton.setBounds(buttonRow.removeFromLeft(100));
    parameterListBox.setBounds(leftPanel.removeFromTop(150));
    leftPanel.removeFromTop(10);
    parameterConfigViewport.setBounds(leftPanel);

    // Right: Measurement config
    measurementConfig->setBounds(contentArea);

    bounds.removeFromTop(10);

    // Bottom: Output and run
    auto outputSection = bounds.removeFromTop(50);
    outputPathLabel.setBounds(outputSection.removeFromTop(25));
    auto outputRow = outputSection.removeFromTop(30);
    outputPathEditor.setBounds(outputRow.removeFromLeft(600));
    outputRow.removeFromLeft(10);
    browseOutputButton.setBounds(outputRow.removeFromLeft(100));
    outputRow.removeFromLeft(10);
    runMeasurementButton.setBounds(outputRow.removeFromLeft(150));

    bounds.removeFromTop(10);

    // Progress
    progressLabel.setBounds(bounds.removeFromTop(25));
    progressBar.setBounds(bounds.removeFromTop(30));
}

void MainComponent::buttonClicked(juce::Button* button) {
    if (button == &browseButton) {
        auto chooser = std::make_shared<juce::FileChooser>("Select VST3 Plugin", juce::File(), "*.vst3");
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync(chooserFlags, [this, chooser](const juce::FileChooser& fc) {
            if (fc.getResults().size() > 0) {
                pluginPathEditor.setText(fc.getResult().getFullPathName(), juce::dontSendNotification);
            }
        });
    } else if (button == &browseOutputButton) {
        auto chooser = std::make_shared<juce::FileChooser>("Select Output Directory", juce::File(outputPathEditor.getText()));
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;
        chooser->launchAsync(chooserFlags, [this, chooser](const juce::FileChooser& fc) {
            if (fc.getResults().size() > 0) {
                outputPathEditor.setText(fc.getResult().getFullPathName(), juce::dontSendNotification);
            }
        });
    } else if (button == &loadPluginButton) {
        loadPlugin();
    } else if (button == &selectAllButton) {
        std::fill(selectedParameters.begin(), selectedParameters.end(), true);
        parameterListBox.updateContent();
    } else if (button == &deselectAllButton) {
        std::fill(selectedParameters.begin(), selectedParameters.end(), false);
        parameterListBox.updateContent();
    } else if (button == &runMeasurementButton) {
        runMeasurement();
    }
}

void MainComponent::textEditorTextChanged(juce::TextEditor& editor) {
    // Handle text changes if needed
}

int MainComponent::getNumRows() {
    return (int)availableParameters.size();
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) {
    if (rowNumber < 0 || rowNumber >= (int)availableParameters.size())
        return;

    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll(rowNumber % 2 == 0 ? juce::Colours::white : juce::Colours::lightgrey);

    g.setColour(juce::Colours::black);
    g.setFont(14.0f);

    juce::String text = availableParameters[rowNumber];
    if (rowNumber < (int)selectedParameters.size() && selectedParameters[rowNumber])
        text = "✓ " + text;

    g.drawText(text, 5, 0, width - 5, height, juce::Justification::centredLeft);
}

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent& e) {
    if (row >= 0 && row < (int)selectedParameters.size()) {
        selectedParameters[row] = !selectedParameters[row];
        parameterListBox.updateContent();
        updateParameterList();
    }
}

void MainComponent::loadPlugin() {
    juce::String pluginPath = pluginPathEditor.getText();
    if (pluginPath.isEmpty()) {
        showError("Please specify a plugin path");
        return;
    }

    juce::File pluginFile(pluginPath);
    // VST3 plugins on macOS are bundles (directories), not files
    if (!pluginFile.exists()) {
        showError("Plugin file does not exist: " + pluginPath);
        return;
    }

    progressLabel.setText("Loading plugin...", juce::dontSendNotification);

    // Get sample rate and block size from measurement config
    Config tempConfig;
    measurementConfig->fillConfig(tempConfig);

    pluginInstance = loadPluginInstance(pluginFile, tempConfig.sampleRate, tempConfig.blockSize);

    if (pluginInstance == nullptr) {
        showError("Failed to load plugin");
        return;
    }

    pluginInfoLabel.setText("Loaded: " + pluginInstance->getName(), juce::dontSendNotification);
    scanPluginParameters();
    runMeasurementButton.setEnabled(true);
    progressLabel.setText("Plugin loaded successfully", juce::dontSendNotification);
}

void MainComponent::scanPluginParameters() {
    if (pluginInstance == nullptr)
        return;

    parameterMap = buildParameterMap(*pluginInstance);
    availableParameters.clear();
    selectedParameters.clear();

    for (const auto& [name, param] : parameterMap) {
        availableParameters.push_back(name);
        selectedParameters.push_back(false);
    }

    std::sort(availableParameters.begin(), availableParameters.end());
    parameterListBox.updateContent();
    updateParameterList();
}

void MainComponent::updateParameterList() {
    // Remove old config components
    parameterConfigComponents.clear();
    parameterConfigContainer.removeAllChildren();

    // Create config components for selected parameters
    int y = 10;
    for (size_t i = 0; i < availableParameters.size(); ++i) {
        if (i < selectedParameters.size() && selectedParameters[i]) {
            auto* comp = new ParameterConfigComponent(availableParameters[i]);
            comp->setBounds(10, y, 480, 150);
            parameterConfigContainer.addAndMakeVisible(comp);
            parameterConfigComponents.push_back(std::unique_ptr<ParameterConfigComponent>(comp));
            y += 160;
        }
    }

    parameterConfigContainer.setSize(500, y);
    parameterConfigViewport.setViewPosition(0, 0);
}

void MainComponent::runMeasurement() {
    if (pluginInstance == nullptr) {
        showError("No plugin loaded");
        return;
    }

    // Count selected parameters
    int selectedCount = 0;
    for (bool selected : selectedParameters) {
        if (selected)
            selectedCount++;
    }

    if (selectedCount == 0) {
        showError("Please select at least one parameter");
        return;
    }

    juce::String outputPath = outputPathEditor.getText();
    if (outputPath.isEmpty()) {
        showError("Please specify an output path");
        return;
    }

    juce::File outDir(outputPath);
    if (!outDir.exists()) {
        outDir.createDirectory();
    }

    if (!outDir.isDirectory()) {
        showError("Output path is not a directory");
        return;
    }

    // Build config from UI
    Config config = buildConfigFromUI();
    config.pluginPath = pluginPathEditor.getText();

    // Run measurement in background thread
    runMeasurementButton.setEnabled(false);
    progressLabel.setText("Running measurement...", juce::dontSendNotification);

    std::thread([this, config, outDir]() {
        try {
            // Build parameter name list
            std::vector<juce::String> paramNames;
            for (size_t i = 0; i < availableParameters.size(); ++i) {
                if (i < selectedParameters.size() && selectedParameters[i]) {
                    paramNames.push_back(availableParameters[i]);
                }
            }

            // Build run grid
            auto runs = buildRunGrid(config, paramNames);

            // Create analyzers
            auto analyzers = createAnalyzers(config, outDir, paramNames);

            // Run measurements
            int64_t totalSamples = (int64_t)(config.seconds * config.sampleRate);
            runMeasurementGrid(*pluginInstance, config.sampleRate, config.blockSize, totalSamples, runs, analyzers,
                               config, outDir);

            // Finish analyzers
            for (auto& analyzer : analyzers) {
                analyzer->finish(outDir);
            }

            juce::MessageManager::callAsync([this]() {
                progressLabel.setText("Measurement complete!", juce::dontSendNotification);
                progress = 1.0;
                progressBar.repaint();
                runMeasurementButton.setEnabled(true);
            });
        } catch (const std::exception& e) {
            juce::MessageManager::callAsync([this, e]() {
                showError("Error: " + juce::String(e.what()));
                runMeasurementButton.setEnabled(true);
            });
        }
    }).detach();
}

Config MainComponent::buildConfigFromUI() {
    Config config;

    // Fill measurement config
    measurementConfig->fillConfig(config);

    // Add parameter buckets from selected parameters
    for (size_t i = 0; i < availableParameters.size(); ++i) {
        if (i < selectedParameters.size() && selectedParameters[i]) {
            // Find corresponding config component
            for (const auto& comp : parameterConfigComponents) {
                if (comp->getConfig().paramName == availableParameters[i]) {
                    config.parameterBuckets.push_back(comp->getConfig());
                    break;
                }
            }
        }
    }

    return config;
}

void MainComponent::showError(const juce::String& message) {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", message);
    progressLabel.setText(message, juce::dontSendNotification);
}
