#include "MeasurementConfigComponent.h"

MeasurementConfigComponent::MeasurementConfigComponent() {
    signalTypeLabel.setText("Signal Type:", juce::dontSendNotification);
    addAndMakeVisible(signalTypeLabel);

    signalTypeCombo.addItem("Sine", 1);
    signalTypeCombo.addItem("Noise", 2);
    signalTypeCombo.addItem("Sweep", 3);
    signalTypeCombo.setSelectedId(1);
    signalTypeCombo.addListener(this);
    addAndMakeVisible(signalTypeCombo);

    sineFreqLabel.setText("Sine Frequency (Hz):", juce::dontSendNotification);
    addAndMakeVisible(sineFreqLabel);
    sineFreqEditor.setText("1000.0", juce::dontSendNotification);
    sineFreqEditor.addListener(this);
    addAndMakeVisible(sineFreqEditor);

    sweepStartLabel.setText("Sweep Start (Hz):", juce::dontSendNotification);
    addAndMakeVisible(sweepStartLabel);
    sweepStartEditor.setText("20.0", juce::dontSendNotification);
    sweepStartEditor.addListener(this);
    addAndMakeVisible(sweepStartEditor);

    sweepEndLabel.setText("Sweep End (Hz):", juce::dontSendNotification);
    addAndMakeVisible(sweepEndLabel);
    sweepEndEditor.setText("20000.0", juce::dontSendNotification);
    sweepEndEditor.addListener(this);
    addAndMakeVisible(sweepEndEditor);

    sampleRateLabel.setText("Sample Rate:", juce::dontSendNotification);
    addAndMakeVisible(sampleRateLabel);
    sampleRateEditor.setText("48000", juce::dontSendNotification);
    sampleRateEditor.addListener(this);
    addAndMakeVisible(sampleRateEditor);

    secondsLabel.setText("Duration (seconds):", juce::dontSendNotification);
    addAndMakeVisible(secondsLabel);
    secondsEditor.setText("5.0", juce::dontSendNotification);
    secondsEditor.addListener(this);
    addAndMakeVisible(secondsEditor);

    blockSizeLabel.setText("Block Size:", juce::dontSendNotification);
    addAndMakeVisible(blockSizeLabel);
    blockSizeEditor.setText("256", juce::dontSendNotification);
    blockSizeEditor.addListener(this);
    addAndMakeVisible(blockSizeEditor);

    inputGainLabel.setText("Input Gain Buckets (dB, comma-separated):", juce::dontSendNotification);
    addAndMakeVisible(inputGainLabel);
    inputGainEditor.setText("-24.0, -18.0, -12.0", juce::dontSendNotification);
    inputGainEditor.addListener(this);
    addAndMakeVisible(inputGainEditor);

    analyzersLabel.setText("Analyzers:", juce::dontSendNotification);
    addAndMakeVisible(analyzersLabel);

    rawCsvButton.setButtonText("Raw CSV");
    rawCsvButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(rawCsvButton);

    rmsPeakButton.setButtonText("RMS/Peak");
    rmsPeakButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(rmsPeakButton);

    transferCurveButton.setButtonText("Transfer Curve");
    transferCurveButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(transferCurveButton);

    linearResponseButton.setButtonText("Linear Response");
    linearResponseButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(linearResponseButton);

    thdButton.setButtonText("THD");
    thdButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(thdButton);

    updateUI();
}

MeasurementConfigComponent::~MeasurementConfigComponent() {}

void MeasurementConfigComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);
}

void MeasurementConfigComponent::resized() {
    auto bounds = getLocalBounds().reduced(10);
    int rowHeight = 30;
    int y = 10;

    auto signalRow = bounds.removeFromTop(rowHeight);
    signalTypeLabel.setBounds(signalRow.removeFromLeft(120));
    signalTypeCombo.setBounds(signalRow.removeFromLeft(150));
    bounds.removeFromTop(5);

    sineFreqLabel.setBounds(bounds.removeFromTop(rowHeight).removeFromLeft(150));
    sineFreqEditor.setBounds(bounds.removeFromTop(rowHeight).removeFromLeft(100));
    bounds.removeFromTop(5);

    auto sweepRow = bounds.removeFromTop(rowHeight);
    sweepStartLabel.setBounds(sweepRow.removeFromLeft(150));
    sweepStartEditor.setBounds(sweepRow.removeFromLeft(100));
    sweepRow.removeFromLeft(20);
    sweepEndLabel.setBounds(sweepRow.removeFromLeft(150));
    sweepEndEditor.setBounds(sweepRow.removeFromLeft(100));
    bounds.removeFromTop(5);

    auto audioRow = bounds.removeFromTop(rowHeight);
    sampleRateLabel.setBounds(audioRow.removeFromLeft(120));
    sampleRateEditor.setBounds(audioRow.removeFromLeft(100));
    audioRow.removeFromLeft(20);
    secondsLabel.setBounds(audioRow.removeFromLeft(120));
    secondsEditor.setBounds(audioRow.removeFromLeft(100));
    audioRow.removeFromLeft(20);
    blockSizeLabel.setBounds(audioRow.removeFromLeft(100));
    blockSizeEditor.setBounds(audioRow.removeFromLeft(100));
    bounds.removeFromTop(5);

    inputGainLabel.setBounds(bounds.removeFromTop(rowHeight));
    inputGainEditor.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5);

    analyzersLabel.setBounds(bounds.removeFromTop(rowHeight));
    auto analyzerRow = bounds.removeFromTop(rowHeight);
    rawCsvButton.setBounds(analyzerRow.removeFromLeft(100));
    analyzerRow.removeFromLeft(10);
    rmsPeakButton.setBounds(analyzerRow.removeFromLeft(100));
    analyzerRow.removeFromLeft(10);
    transferCurveButton.setBounds(analyzerRow.removeFromLeft(120));
    analyzerRow.removeFromLeft(10);
    linearResponseButton.setBounds(analyzerRow.removeFromLeft(120));
    analyzerRow.removeFromLeft(10);
    thdButton.setBounds(analyzerRow.removeFromLeft(100));
}

void MeasurementConfigComponent::comboBoxChanged(juce::ComboBox* comboBox) {
    if (comboBox == &signalTypeCombo) {
        updateUI();
    }
}

void MeasurementConfigComponent::textEditorTextChanged(juce::TextEditor& editor) {
    // Values are read when building config
}

void MeasurementConfigComponent::buttonClicked(juce::Button* button) {
    // Analyzer toggles are handled automatically
}

void MeasurementConfigComponent::updateUI() {
    int signalType = signalTypeCombo.getSelectedId();
    bool showSine = (signalType == 1);
    bool showSweep = (signalType == 3);

    sineFreqLabel.setVisible(showSine);
    sineFreqEditor.setVisible(showSine);
    sweepStartLabel.setVisible(showSweep);
    sweepStartEditor.setVisible(showSweep);
    sweepEndLabel.setVisible(showSweep);
    sweepEndEditor.setVisible(showSweep);

    resized();
}

void MeasurementConfigComponent::fillConfig(Config& config) {
    int signalType = signalTypeCombo.getSelectedId();
    if (signalType == 1)
        config.signalType = "sine";
    else if (signalType == 2)
        config.signalType = "noise";
    else if (signalType == 3)
        config.signalType = "sweep";

    config.sineFrequency = sineFreqEditor.getText().getDoubleValue();
    config.sweepStartHz = sweepStartEditor.getText().getDoubleValue();
    config.sweepEndHz = sweepEndEditor.getText().getDoubleValue();

    config.sampleRate = sampleRateEditor.getText().getDoubleValue();
    config.seconds = secondsEditor.getText().getDoubleValue();
    config.blockSize = blockSizeEditor.getText().getIntValue();

    // Parse input gain buckets
    juce::StringArray tokens;
    tokens.addTokens(inputGainEditor.getText(), ",", "");
    config.inputGainBucketsDb.clear();
    for (const auto& token : tokens) {
        float val = (float)token.trim().getDoubleValue();
        config.inputGainBucketsDb.push_back(val);
    }

    // Analyzers
    config.analyzers.clear();
    if (rawCsvButton.getToggleState())
        config.analyzers.push_back("RawCsv");
    if (rmsPeakButton.getToggleState())
        config.analyzers.push_back("RmsPeak");
    if (transferCurveButton.getToggleState())
        config.analyzers.push_back("TransferCurve");
    if (linearResponseButton.getToggleState())
        config.analyzers.push_back("LinearResponse");
    if (thdButton.getToggleState())
        config.analyzers.push_back("Thd");
}

void MeasurementConfigComponent::loadFromConfig(const Config& config) {
    if (config.signalType == "sine")
        signalTypeCombo.setSelectedId(1);
    else if (config.signalType == "noise")
        signalTypeCombo.setSelectedId(2);
    else if (config.signalType == "sweep")
        signalTypeCombo.setSelectedId(3);

    sineFreqEditor.setText(juce::String(config.sineFrequency), juce::dontSendNotification);
    sweepStartEditor.setText(juce::String(config.sweepStartHz), juce::dontSendNotification);
    sweepEndEditor.setText(juce::String(config.sweepEndHz), juce::dontSendNotification);

    sampleRateEditor.setText(juce::String(config.sampleRate), juce::dontSendNotification);
    secondsEditor.setText(juce::String(config.seconds), juce::dontSendNotification);
    blockSizeEditor.setText(juce::String(config.blockSize), juce::dontSendNotification);

    juce::String gainStr;
    for (size_t i = 0; i < config.inputGainBucketsDb.size(); ++i) {
        if (i > 0)
            gainStr += ", ";
        gainStr += juce::String(config.inputGainBucketsDb[i]);
    }
    inputGainEditor.setText(gainStr, juce::dontSendNotification);

    auto hasAnalyzer = [&](const juce::String& name) {
        return std::find(config.analyzers.begin(), config.analyzers.end(), name) != config.analyzers.end();
    };

    rawCsvButton.setToggleState(hasAnalyzer("RawCsv"), juce::dontSendNotification);
    rmsPeakButton.setToggleState(hasAnalyzer("RmsPeak"), juce::dontSendNotification);
    transferCurveButton.setToggleState(hasAnalyzer("TransferCurve"), juce::dontSendNotification);
    linearResponseButton.setToggleState(hasAnalyzer("LinearResponse"), juce::dontSendNotification);
    thdButton.setToggleState(hasAnalyzer("Thd"), juce::dontSendNotification);

    updateUI();
}
