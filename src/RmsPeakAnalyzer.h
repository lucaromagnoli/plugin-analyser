#pragma once

#include "Analyzer.h"
#include "JuceHeader.h"
#include <map>
#include <vector>

struct RunStats {
    double sumSqInL = 0.0;
    double sumSqInR = 0.0;
    double sumSqOutL = 0.0;
    double sumSqOutR = 0.0;
    float peakInL = 0.0f;
    float peakInR = 0.0f;
    float peakOutL = 0.0f;
    float peakOutR = 0.0f;
    int64_t sampleCount = 0;
};

struct RmsPeakAnalyzer : public Analyzer {
    RmsPeakAnalyzer(const juce::File& outDir, const std::vector<juce::String>& paramNames);
    ~RmsPeakAnalyzer() override;

    void processBlock(const BlockContext& ctx) override;
    void finish(const juce::File& outDir) override;

private:
    std::map<int, RunStats> perRunStats;
    std::map<int, std::map<juce::String, float>> runParamValues; // runId -> paramName -> value
    std::map<int, float> runInputGainDb;                         // runId -> inputGainDb
    std::vector<juce::String> paramNames;
    juce::File outputDir;
};

std::unique_ptr<Analyzer> createRmsPeakAnalyzer(const juce::File& outDir, const std::vector<juce::String>& paramNames);
