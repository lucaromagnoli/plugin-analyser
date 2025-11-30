#pragma once

#include "Analyzer.h"
#include <JuceHeader.h>
#include <fstream>
#include <memory>

struct RawCsvAnalyzer : public Analyzer {
    RawCsvAnalyzer(const juce::File& outDir);
    ~RawCsvAnalyzer() override;

    void processBlock(const BlockContext& ctx) override;
    void finish(const juce::File& outDir) override;

private:
    std::unique_ptr<std::ofstream> csvFile;
    bool headerWritten = false;
};

std::unique_ptr<Analyzer> createRawCsvAnalyzer(const juce::File& outDir);
