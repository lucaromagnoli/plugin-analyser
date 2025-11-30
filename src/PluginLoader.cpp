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

    // For VST3 bundles on macOS, we need to check the bundle structure
    juce::String pluginPath = pluginFile.getFullPathName();

    // If it's a .vst3 bundle, ensure we're pointing to the bundle directory
    if (pluginFile.isDirectory() && pluginPath.endsWithIgnoreCase(".vst3")) {
        // VST3 bundle is correct - use as-is
    } else if (pluginFile.isDirectory()) {
        // Might be pointing inside the bundle, try to find the .vst3 parent
        juce::File current = pluginFile;
        while (!current.isRoot() && !current.getFileName().endsWithIgnoreCase(".vst3")) {
            current = current.getParentDirectory();
        }
        if (current.getFileName().endsWithIgnoreCase(".vst3")) {
            pluginPath = current.getFullPathName();
        }
    }

    // Debug: Check what formats were added
    int numFormats = formatManager.getNumFormats();
    std::cerr << "Format manager has " << numFormats << " format(s)" << std::endl;
    for (int i = 0; i < numFormats; ++i) {
        std::cerr << "  Format " << i << ": " << formatManager.getFormat(i)->getName() << std::endl;
    }

    // Try to find the plugin by scanning
    bool foundFormat = false;

    for (int i = 0; i < numFormats; ++i) {
        auto* format = formatManager.getFormat(i);
        juce::String formatName = format->getName();

        if (format->fileMightContainThisPluginType(pluginPath)) {
            foundFormat = true;
            juce::OwnedArray<juce::PluginDescription> found;
            format->findAllTypesForFile(found, pluginPath);
            if (found.size() > 0) {
                description = *found[0];
                std::cerr << "Found plugin: " << description.name << " (" << formatName << ")" << std::endl;
                break;
            } else {
                std::cerr << "Format " << formatName << " recognized file but found no plugins" << std::endl;
            }
        }
    }

    if (!foundFormat) {
        errorMessageOut = "No plugin format recognized for: " + pluginPath +
                          "\nMake sure the file is a valid VST3 plugin bundle (.vst3 directory).";
        std::cerr << errorMessageOut << std::endl;
        std::cerr << "Available formats: ";
        for (int i = 0; i < numFormats; ++i) {
            std::cerr << formatManager.getFormat(i)->getName();
            if (i < numFormats - 1)
                std::cerr << ", ";
        }
        std::cerr << std::endl;
        return nullptr;
    }

    if (description.name.isEmpty()) {
        errorMessageOut = "Failed to get plugin description for: " + pluginPath +
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

std::map<juce::String, juce::AudioProcessorParameter*> buildParameterMap(juce::AudioPluginInstance& plugin,
                                                                         bool uiOnly) {
    std::map<juce::String, juce::AudioProcessorParameter*> paramMap;

    for (auto* param : plugin.getParameters()) {
        if (param == nullptr)
            continue;

        // Filter out non-UI parameters if requested
        if (uiOnly) {
            // Skip meta parameters (internal automation)
            if (param->isMetaParameter())
                continue;

            // Skip non-automatable parameters (typically internal)
            if (!param->isAutomatable())
                continue;

            // Skip MIDI CC parameters (they're not UI-exposed)
            juce::String paramName = param->getName(512);
            if (paramName.containsIgnoreCase("MIDI CC") || paramName.containsIgnoreCase("midi cc"))
                continue;
        }

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
