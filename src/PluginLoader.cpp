#include "PluginLoader.h"
#include <iostream>

std::unique_ptr<juce::AudioPluginInstance> loadPluginInstance(const juce::File& pluginFile, double sampleRate,
                                                              int blockSize, juce::String& errorMessageOut) {
    errorMessageOut.clear();

    // VST3 plugins on macOS are bundles (directories), not files
    if (!pluginFile.exists()) {
        errorMessageOut = "Plugin file does not exist: " + pluginFile.getFullPathName();
        std::cerr << errorMessageOut << std::endl;
        return nullptr;
    }

    juce::AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    juce::String errorMessage;
    juce::PluginDescription description;

    // Try to find the plugin by scanning
    bool foundFormat = false;
    for (int i = 0; i < formatManager.getNumFormats(); ++i) {
        auto* format = formatManager.getFormat(i);
        if (format->fileMightContainThisPluginType(pluginFile.getFullPathName())) {
            foundFormat = true;
            juce::OwnedArray<juce::PluginDescription> found;
            format->findAllTypesForFile(found, pluginFile.getFullPathName());
            if (found.size() > 0) {
                description = *found[0];
                break;
            }
        }
    }

    if (!foundFormat) {
        errorMessageOut = "No plugin format recognized for: " + pluginFile.getFullPathName() +
                         "\nMake sure the file is a valid VST3 plugin.";
        std::cerr << errorMessageOut << std::endl;
        return nullptr;
    }

    if (description.name.isEmpty()) {
        errorMessageOut = "Failed to get plugin description for: " + pluginFile.getFullPathName() +
                         "\nThe file may be corrupted or not a valid plugin.";
        std::cerr << errorMessageOut << std::endl;
        return nullptr;
    }

    auto instance = formatManager.createPluginInstance(description, sampleRate, blockSize, errorMessage);

    if (instance == nullptr) {
        errorMessageOut = "Failed to create plugin instance: " + errorMessage;
        std::cerr << errorMessageOut << std::endl;
        return nullptr;
    }

    instance->prepareToPlay(sampleRate, blockSize);

    return instance;
}

std::map<juce::String, juce::AudioProcessorParameter*> buildParameterMap(juce::AudioPluginInstance& plugin) {
    std::map<juce::String, juce::AudioProcessorParameter*> paramMap;

    for (auto* param : plugin.getParameters()) {
        if (param == nullptr)
            continue;

        juce::String paramName = param->getName(512);
        paramName = paramName.trim().toLowerCase();
        paramMap[paramName] = param;
    }

    return paramMap;
}

void setParameterValue(juce::AudioPluginInstance& plugin,
                       const std::map<juce::String, juce::AudioProcessorParameter*>& paramMap,
                       const juce::String& paramName, float normalizedValue) {
    juce::String searchName = paramName.trim().toLowerCase();
    auto it = paramMap.find(searchName);

    if (it == paramMap.end()) {
        std::cerr << "Warning: Parameter not found: " << paramName << std::endl;
        return;
    }

    it->second->setValueNotifyingHost(normalizedValue);
}
