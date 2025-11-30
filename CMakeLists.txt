cmake_minimum_required(VERSION 3.22)
project(plugin-analyser VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE setup
# Note: You'll need to set JUCE_DIR or use FetchContent
# For now, we'll assume JUCE is available via find_package or FetchContent

# Try to use local JUCE installation first (faster builds)
set(JUCE_DIR "~/JUCE")
get_filename_component(JUCE_DIR ${JUCE_DIR} ABSOLUTE)

option(PLUGIN_ANALYSER_USE_FETCH_JUCE "Use FetchContent to get JUCE if local not found" ON)

if(EXISTS ${JUCE_DIR}/CMakeLists.txt)
    message(STATUS "Using local JUCE installation at ${JUCE_DIR}")
    add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/juce)
    set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF)
    set(JUCE_BUILD_EXTRAS OFF)
    set(JUCE_BUILD_EXAMPLES OFF)
elseif(PLUGIN_ANALYSER_USE_FETCH_JUCE)
    message(STATUS "Local JUCE not found at ${JUCE_DIR}, downloading via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.0
    )
    FetchContent_MakeAvailable(juce)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

# Source files
set(SOURCES
    src/Main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/MainComponent.cpp
    src/MainComponent.h
    src/ParameterConfigComponent.cpp
    src/ParameterConfigComponent.h
    src/MeasurementConfigComponent.cpp
    src/MeasurementConfigComponent.h
    src/Config.cpp
    src/Config.h
    src/PluginLoader.cpp
    src/PluginLoader.h
    src/SignalGenerator.cpp
    src/SignalGenerator.h
    src/BucketSpec.cpp
    src/BucketSpec.h
    src/RunConfig.h
    src/BlockContext.h
    src/Analyzer.h
    src/RawCsvAnalyzer.cpp
    src/RawCsvAnalyzer.h
    src/RmsPeakAnalyzer.cpp
    src/RmsPeakAnalyzer.h
    src/TransferCurveAnalyzer.cpp
    src/TransferCurveAnalyzer.h
    src/LinearResponseAnalyzer.cpp
    src/LinearResponseAnalyzer.h
    src/ThdAnalyzer.cpp
    src/ThdAnalyzer.h
    src/MeasurementEngine.cpp
    src/MeasurementEngine.h
)

# Create GUI application
add_executable(PluginAnalyser MACOSX_BUNDLE ${SOURCES})

target_compile_definitions(PluginAnalyser
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_DARK_SPLASH_SCREEN=0
        JUCE_PLUGINHOST_VST3=1
)

target_link_libraries(PluginAnalyser
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_dsp
        juce::juce_core
        juce::juce_data_structures
        juce::juce_gui_basics
        juce::juce_gui_extra
)

# Set macOS bundle properties for proper app bundle with dock icon
set_target_properties(PluginAnalyser PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_STRING "Plugin Analyser - VST3 Plugin Analysis Tool"
    MACOSX_BUNDLE_ICON_FILE ""
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.pluginanalyser.app"
    MACOSX_BUNDLE_BUNDLE_NAME "Plugin Analyser"
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024"
    OUTPUT_NAME "Plugin Analyser"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.pluginanalyser.app"
    XCODE_ATTRIBUTE_LSMinimumSystemVersion "12.0"
)

# Also create console version for CLI usage
add_executable(plugin_measure_grid_cli src/main_cli.cpp
    src/Config.cpp src/Config.h
    src/PluginLoader.cpp src/PluginLoader.h
    src/SignalGenerator.cpp src/SignalGenerator.h
    src/BucketSpec.cpp src/BucketSpec.h
    src/RunConfig.h src/BlockContext.h src/Analyzer.h
    src/RawCsvAnalyzer.cpp src/RawCsvAnalyzer.h
    src/RmsPeakAnalyzer.cpp src/RmsPeakAnalyzer.h
    src/TransferCurveAnalyzer.cpp src/TransferCurveAnalyzer.h
    src/LinearResponseAnalyzer.cpp src/LinearResponseAnalyzer.h
    src/ThdAnalyzer.cpp src/ThdAnalyzer.h
    src/MeasurementEngine.cpp src/MeasurementEngine.h
)

target_compile_definitions(plugin_measure_grid_cli
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_PLUGINHOST_VST3=1
)

target_link_libraries(plugin_measure_grid_cli
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_dsp
        juce::juce_core
        juce::juce_data_structures
)

# For JSON parsing, we'll use JUCE's JSON class or add nlohmann/json
# JUCE has built-in JSON support via juce::var and juce::JSON
