#pragma once

#include "Analyzer.h"
#include <JuceHeader.h>
#include <map>
#include <vector>
#include <complex>

struct ThdAnalyzer : public Analyzer
{
    ThdAnalyzer(const juce::File& outDir, int fftSize, double fundamentalFreq, const std::vector<juce::String>& paramNames);
    ~ThdAnalyzer() override;
    
    void processBlock(const BlockContext& ctx) override;
    void finish(const juce::File& outDir) override;
    
private:
    struct RunThdData
    {
        std::vector<float> buffer;
        std::vector<std::pair<int64, double>> thdResults;  // (centreSample, thd)
        std::map<juce::String, float> paramValues;
        float inputGainDb;
        double sampleRate = 48000.0;
    };
    
    std::map<int, RunThdData> perRunData;
    int fftSize;
    double fundamentalFreq;
    std::vector<juce::String> paramNames;
    juce::File outputDir;
    
    void processFFTWindow(RunThdData& data, int64 centreSample);
    void applyHannWindow(std::vector<float>& buffer);
    double computeTHD(const std::vector<std::complex<float>>& fftResult, double sampleRate);
};

std::unique_ptr<Analyzer> createThdAnalyzer(const juce::File& outDir, int fftSize, double fundamentalFreq, const std::vector<juce::String>& paramNames);

