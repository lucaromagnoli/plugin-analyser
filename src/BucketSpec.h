#pragma once

#include <JuceHeader.h>
#include <vector>

struct BucketSpec {
    enum class Strategy { ExplicitValues, Linear, Log, EdgeAndCenter };

    juce::String paramName;
    Strategy strategy;
    float min = 0.0f;
    float max = 1.0f;
    int numBuckets = 0;
    std::vector<float> values;

    static Strategy strategyFromString(const juce::String& str);
    std::vector<float> generateValues() const;
};
