#include "RawCsvAnalyzer.h"
#include <iostream>

RawCsvAnalyzer::RawCsvAnalyzer(const juce::File& outDir) {
    juce::File csvFile = outDir.getChildFile("raw.csv");
    this->csvFile = std::make_unique<std::ofstream>(csvFile.getFullPathName().toStdString());
    if (!this->csvFile->is_open()) {
        std::cerr << "Failed to open raw.csv for writing" << std::endl;
        this->csvFile.reset();
    }
}

RawCsvAnalyzer::~RawCsvAnalyzer() {
    if (csvFile && csvFile->is_open())
        csvFile->close();
}

void RawCsvAnalyzer::processBlock(const BlockContext& ctx) {
    if (!csvFile)
        return;

    if (!headerWritten) {
        *csvFile << "runId,sample,time_sec,inL";
        if (ctx.inR != nullptr)
            *csvFile << ",inR";
        *csvFile << ",outL";
        if (ctx.outR != nullptr)
            *csvFile << ",outR";
        *csvFile << "\n";
        headerWritten = true;
    }

    for (int i = 0; i < ctx.numSamples; ++i) {
        int64_t sampleIndex = ctx.firstSample + i;
        double timeSec = (double)sampleIndex / ctx.sampleRate;

        *csvFile << ctx.runId << "," << sampleIndex << "," << timeSec << "," << ctx.inL[i];
        if (ctx.inR != nullptr)
            *csvFile << "," << ctx.inR[i];
        *csvFile << "," << ctx.outL[i];
        if (ctx.outR != nullptr)
            *csvFile << "," << ctx.outR[i];
        *csvFile << "\n";
    }
}

void RawCsvAnalyzer::finish(const juce::File& outDir) {
    if (csvFile && csvFile->is_open()) {
        csvFile->close();
        csvFile.reset();
    }
}

std::unique_ptr<Analyzer> createRawCsvAnalyzer(const juce::File& outDir) {
    return std::make_unique<RawCsvAnalyzer>(outDir);
}
