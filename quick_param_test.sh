#!/bin/bash
PLUGIN="/Volumes/External SSD/Plug-Ins/VST3/Acustica/GAINSTATION2.vst3"
echo "Testing plugin: $PLUGIN"
echo "Plugin exists: $([ -d "$PLUGIN" ] && echo "yes" || echo "no")"
echo ""
echo "Loading plugin..."
juce::String errorMessage;
auto plugin = loadPluginInstance(pluginFile, 48000.0, 256, errorMessage);
if (plugin == nullptr) {
    std::cerr << "Error: " << errorMessage << std::endl;
    return 1;
}
auto paramMap = buildParameterMap(*plugin);
for (const auto& [name, param] : paramMap) {
    std::cout << name << std::endl;
}
