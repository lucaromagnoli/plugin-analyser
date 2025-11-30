#pragma once

#include "Config.h"
#include "RunConfig.h"
#include "Analyzer.h"
#include "SignalGenerator.h"
#include <JuceHeader.h>
#include <vector>
#include <memory>

std::vector<RunConfig> buildRunGrid(const Config& config, const std::vector<juce::String>& paramNames);

void runMeasurementGrid(
    juce::AudioPluginInstance& plugin,
    double sampleRate,
    int blockSize,
    int64 totalSamples,
    const std::vector<RunConfig>& runs,
    const std::vector<std::unique_ptr<Analyzer>>& analyzers,
    const Config& config,
    const juce::File& outDir);

std::vector<std::unique_ptr<Analyzer>> createAnalyzers(
    const Config& config,
    const juce::File& outDir,
    const std::vector<juce::String>& paramNames);

