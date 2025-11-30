#pragma once

#include "BlockContext.h"
#include "JuceHeader.h"

struct Analyzer {
    virtual ~Analyzer() = default;
    virtual void processBlock(const BlockContext& ctx) = 0;
    virtual void finish(const juce::File& outDir) {}
};
