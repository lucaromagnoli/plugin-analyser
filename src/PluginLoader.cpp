#include "PluginLoader.h"
#include <iostream>

std::unique_ptr<juce::AudioPluginInstance> loadPluginInstance(
    const juce::File& pluginFile,
    double sampleRate,
    int blockSize)
{
    if (!pluginFile.existsAsFile())
    {
        std::cerr << "Plugin file does not exist: " << pluginFile.getFullPathName() << std::endl;
        return nullptr;
    }
    
    juce::AudioPluginFormatManager formatManager;
    formatManager.addFormat(new juce::VST3PluginFormat());
    
    juce::String errorMessage;
    juce::PluginDescription description;
    
    // Try to find the plugin by scanning
    for (int i = 0; i < formatManager.getNumFormats(); ++i)
    {
        auto* format = formatManager.getFormat(i);
        if (format->fileMightContainThisPluginType(pluginFile.getFullPathName()))
        {
            juce::OwnedArray<juce::PluginDescription> found;
            format->findAllTypesForFile(found, pluginFile.getFullPathName());
            if (found.size() > 0)
            {
                description = *found[0];
                break;
            }
        }
    }
    
    if (description.name.isEmpty())
    {
        std::cerr << "Failed to get plugin description for: " << pluginFile.getFullPathName() << std::endl;
        return nullptr;
    }
    
    auto instance = formatManager.createPluginInstance(description, sampleRate, blockSize, errorMessage);
    
    if (instance == nullptr)
    {
        std::cerr << "Failed to create plugin instance: " << errorMessage << std::endl;
        return nullptr;
    }
    
    instance->prepareToPlay(sampleRate, blockSize);
    
    return instance;
}

std::map<juce::String, juce::AudioProcessorParameter*> buildParameterMap(
    juce::AudioPluginInstance& plugin)
{
    std::map<juce::String, juce::AudioProcessorParameter*> paramMap;
    
    auto* processor = plugin.getAudioProcessor();
    if (processor == nullptr)
        return paramMap;
    
    for (auto* param : processor->getParameters())
    {
        if (param == nullptr)
            continue;
        
        juce::String paramName = param->getName(512);
        paramName = paramName.trim().toLowerCase();
        paramMap[paramName] = param;
    }
    
    return paramMap;
}

void setParameterValue(
    juce::AudioPluginInstance& plugin,
    const std::map<juce::String, juce::AudioProcessorParameter*>& paramMap,
    const juce::String& paramName,
    float normalizedValue)
{
    juce::String searchName = paramName.trim().toLowerCase();
    auto it = paramMap.find(searchName);
    
    if (it == paramMap.end())
    {
        std::cerr << "Warning: Parameter not found: " << paramName << std::endl;
        return;
    }
    
    it->second->setValueNotifyingHost(normalizedValue);
}

