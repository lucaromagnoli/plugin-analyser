#include "MainWindow.h"

MainWindow::MainWindow(juce::String name)
    : DocumentWindow(
          name,
          juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
          DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(1200, 800);
#endif

    setVisible(true);
    toFront(true);
}

MainWindow::~MainWindow() {}

void MainWindow::closeButtonPressed() {
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
