#include "Config.h"
#include "JuceHeader.h"
#include "MeasurementEngine.h"
#include "PluginLoader.h"
#include <iostream>
#include <vector>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " --config <path> --out <path> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --config <path>     JSON configuration file (required)\n";
    std::cout << "  --out <path>        Output directory (required)\n";
    std::cout << "  --plugin <path>     Override pluginPath in JSON\n";
    std::cout << "  --seconds N         Override duration in seconds\n";
    std::cout << "  --samplerate SR     Override sample rate\n";
    std::cout << "  --blocksize BS       Override block size\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    juce::String configPath;
    juce::String outPath;
    juce::String pluginPathOverride;
    double secondsOverride = -1.0;
    double sampleRateOverride = -1.0;
    int blockSizeOverride = -1;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        juce::String arg = argv[i];

        if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            outPath = argv[++i];
        } else if (arg == "--plugin" && i + 1 < argc) {
            pluginPathOverride = argv[++i];
        } else if (arg == "--seconds" && i + 1 < argc) {
            secondsOverride = juce::String(argv[++i]).getDoubleValue();
        } else if (arg == "--samplerate" && i + 1 < argc) {
            sampleRateOverride = juce::String(argv[++i]).getDoubleValue();
        } else if (arg == "--blocksize" && i + 1 < argc) {
            blockSizeOverride = juce::String(argv[++i]).getIntValue();
        }
    }

    if (configPath.isEmpty() || outPath.isEmpty()) {
        std::cerr << "Error: --config and --out are required\n";
        printUsage(argv[0]);
        return 1;
    }

    try {
        // Load config
        juce::File configFile(configPath);
        Config config = Config::fromJson(configFile);

        // Apply overrides
        if (!pluginPathOverride.isEmpty())
            config.pluginPath = pluginPathOverride;
        if (secondsOverride > 0.0)
            config.seconds = secondsOverride;
        if (sampleRateOverride > 0.0)
            config.sampleRate = sampleRateOverride;
        if (blockSizeOverride > 0)
            config.blockSize = blockSizeOverride;

        // Create output directory
        juce::File outDir(outPath);
        if (!outDir.exists()) {
            outDir.createDirectory();
        }

        if (!outDir.isDirectory()) {
            std::cerr << "Error: Output path is not a directory: " << outPath << std::endl;
            return 1;
        }

        // Load plugin
        std::cout << "Loading plugin: " << config.pluginPath << std::endl;
        auto plugin = loadPluginInstance(juce::File(config.pluginPath), config.sampleRate, config.blockSize);

        if (plugin == nullptr) {
            std::cerr << "Failed to load plugin" << std::endl;
            return 1;
        }

        std::cout << "Plugin loaded: " << plugin->getName() << std::endl;

        // Build parameter name list
        std::vector<juce::String> paramNames;
        for (const auto& bucket : config.parameterBuckets) {
            paramNames.push_back(bucket.paramName);
        }

        // Build run grid
        std::cout << "Building measurement grid..." << std::endl;
        auto runs = buildRunGrid(config, paramNames);
        std::cout << "Generated " << runs.size() << " measurement runs" << std::endl;

        // Create analyzers
        std::cout << "Creating analyzers..." << std::endl;
        auto analyzers = createAnalyzers(config, outDir, paramNames);
        std::cout << "Created " << analyzers.size() << " analyzers" << std::endl;

        // Run measurements
        int64_t totalSamples = (int64_t)(config.seconds * config.sampleRate);
        std::cout << "Running measurements..." << std::endl;
        runMeasurementGrid(*plugin, config.sampleRate, config.blockSize, totalSamples, runs, analyzers, config, outDir);

        // Finish analyzers
        std::cout << "Finalizing analyzers..." << std::endl;
        for (auto& analyzer : analyzers) {
            analyzer->finish(outDir);
        }

        std::cout << "Measurement complete!" << std::endl;

        plugin->releaseResources();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
