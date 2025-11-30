#include "PluginLoader.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <plugin_path>" << std::endl;
        return 1;
    }

    juce::String pluginPath = argv[1];
    juce::File pluginFile(pluginPath);

    std::cout << "Loading plugin: " << pluginPath << std::endl;

    juce::String errorMessage;
    auto plugin = loadPluginInstance(pluginFile, 48000.0, 256, errorMessage);

    if (plugin == nullptr) {
        std::cerr << "Error: " << errorMessage << std::endl;
        return 1;
    }

    std::cout << "\n✅ Successfully loaded: " << plugin->getName() << std::endl;
    std::cout << "\nParameters:\n";
    std::cout << "==================\n";

    auto paramMap = buildParameterMap(*plugin);
    int idx = 0;
    for (const auto& [name, param] : paramMap) {
        std::cout << idx++ << ". " << name << " (value: " << param->getValue() << ")" << std::endl;
    }

    std::cout << "\nTotal parameters: " << paramMap.size() << std::endl;

    return 0;
}
