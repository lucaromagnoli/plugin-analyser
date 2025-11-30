#pragma once

#include "JuceHeader.h"
#include <map>

struct RunConfig {
    int runId;
    std::map<juce::String, float> paramValues; // paramName -> normalized [0,1] value
    float inputGainDb;
};
