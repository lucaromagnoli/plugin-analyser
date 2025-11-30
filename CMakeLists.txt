cmake_minimum_required(VERSION 3.22)
project(plugin-analyser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE setup
# Note: You'll need to set JUCE_DIR or use FetchContent
# For now, we'll assume JUCE is available via find_package or FetchContent

option(PLUGIN_ANALYSER_USE_FETCH_JUCE "Use FetchContent to get JUCE" ON)

if(PLUGIN_ANALYSER_USE_FETCH_JUCE)
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
    src/main.cpp
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

# Create executable
add_executable(plugin_measure_grid ${SOURCES})

target_compile_definitions(plugin_measure_grid
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_DARK_SPLASH_SCREEN=0
)

target_link_libraries(plugin_measure_grid
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_dsp
        juce::juce_core
        juce::juce_data_structures
)

# Set output name
set_target_properties(plugin_measure_grid PROPERTIES
    OUTPUT_NAME "plugin_measure_grid"
)

# For JSON parsing, we'll use JUCE's JSON class or add nlohmann/json
# JUCE has built-in JSON support via juce::var and juce::JSON

