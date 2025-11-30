#pragma once

#include "JuceHeader.h"
#include <cstdint>

struct SineGenerator {
    double sampleRate = 48000.0;
    double frequency = 1000.0;
    double phase = 0.0;
    float amplitude = 0.5f;

    void fillBlock(juce::AudioBuffer<float>& buffer, int numSamples);
};

struct NoiseGenerator {
    float amplitude = 0.5f;
    juce::Random rng;

    void fillBlock(juce::AudioBuffer<float>& buffer, int numSamples);
};

struct SweepGenerator {
    double sampleRate = 48000.0;
    double startHz = 20.0;
    double endHz = 20000.0;
    double duration = 5.0;
    float amplitude = 0.5f;

    double currentPhase = 0.0;
    double currentFreq = 20.0;
    int64_t currentSample = 0;

    void reset();
    void fillBlock(juce::AudioBuffer<float>& buffer, int numSamples);
};
