#pragma once

#include "JuceHeader.h"
#include <map>
#include <memory>

std::unique_ptr<juce::AudioPluginInstance> loadPluginInstance(const juce::File& pluginFile, double sampleRate,
                                                              int blockSize, juce::String& errorMessageOut);

std::map<juce::String, juce::AudioProcessorParameter*> buildParameterMap(juce::AudioPluginInstance& plugin);

void setParameterValue(juce::AudioPluginInstance& plugin,
                       const std::map<juce::String, juce::AudioProcessorParameter*>& paramMap,
                       const juce::String& paramName, float normalizedValue);
