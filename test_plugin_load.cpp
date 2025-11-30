#include "PluginLoader.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <plugin_path>" << std::endl;
        return 1;
    }

    juce::String pluginPath = argv[1];
    juce::File pluginFile(pluginPath);

    std::cout << "Testing plugin: " << pluginPath << std::endl;
    std::cout << "Exists: " << (pluginFile.exists() ? "yes" : "no") << std::endl;
    std::cout << "Is directory: " << (pluginFile.isDirectory() ? "yes" : "no") << std::endl;

    juce::String errorMessage;
    auto plugin = loadPluginInstance(pluginFile, 48000.0, 256, errorMessage);

    if (plugin == nullptr) {
        std::cerr << "Error: " << errorMessage << std::endl;
        return 1;
    }

    std::cout << "Success! Loaded: " << plugin->getName() << std::endl;
    return 0;
}
