#pragma once

#include "MainComponent.h"
#include <JuceHeader.h>

class MainWindow : public juce::DocumentWindow {
public:
    MainWindow(juce::String name);
    ~MainWindow() override;

    void closeButtonPressed() override;

private:
    std::unique_ptr<MainComponent> mainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
