#pragma once

#include "JuceHeader.h"
#include <memory>

class ParameterInspectionDialog : public juce::DialogWindow {
public:
    ParameterInspectionDialog(juce::AudioPluginInstance* plugin);
    ~ParameterInspectionDialog() override;

    static void show(juce::AudioPluginInstance* plugin);

private:
    class ContentComponent;
    std::unique_ptr<ContentComponent> content;
};
