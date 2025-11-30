#include "MainWindow.h"
#include "JuceHeader.h"

class PluginAnalyserApplication : public juce::JUCEApplication {
public:
    PluginAnalyserApplication() {}

    const juce::String getApplicationName() override {
        return "Plugin Analyser";
    }
    const juce::String getApplicationVersion() override {
        return "1.0.0";
    }
    bool moreThanOneInstanceAllowed() override {
        return true;
    }

    void initialise(const juce::String& commandLine) override {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
        mainWindow->setVisible(true);
    }

    void shutdown() override {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override {
        quit();
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(PluginAnalyserApplication)
