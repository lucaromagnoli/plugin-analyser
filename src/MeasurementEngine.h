#pragma once

#include "Analyzer.h"
#include "Config.h"
#include "JuceHeader.h"
#include "RunConfig.h"
#include "SignalGenerator.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

std::vector<RunConfig> buildRunGrid(const Config& config, const std::vector<juce::String>& paramNames);

void runMeasurementGrid(juce::AudioPluginInstance& plugin, double sampleRate, int blockSize, int64_t totalSamples,
                        const std::vector<RunConfig>& runs, const std::vector<std::unique_ptr<Analyzer>>& analyzers,
                        const Config& config, const juce::File& outDir,
                        std::function<void(int)> progressCallback = nullptr, int numThreads = 1);

std::vector<std::unique_ptr<Analyzer>> createAnalyzers(const Config& config, const juce::File& outDir,
                                                       const std::vector<juce::String>& paramNames);
