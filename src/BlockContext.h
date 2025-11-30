#pragma once

#include <JuceHeader.h>
#include <map>
#include <vector>

struct BlockContext {
    int64 firstSample; // absolute sample index at start of block
    double sampleRate;
    int numSamples;
    const float* inL;
    const float* inR; // may be nullptr
    const float* outL;
    const float* outR;

    // Parameters (normalized [0,1]) for this run in a fixed param order
    std::vector<float> params;

    // Metadata
    int runId;
    std::map<juce::String, float> paramNamedValues; // name -> value
    float inputGainDb;
};
