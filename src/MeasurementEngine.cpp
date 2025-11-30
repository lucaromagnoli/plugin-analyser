#include "MeasurementEngine.h"
#include "BucketSpec.h"
#include "LinearResponseAnalyzer.h"
#include "PluginLoader.h"
#include "RawCsvAnalyzer.h"
#include "RmsPeakAnalyzer.h"
#include "ThdAnalyzer.h"
#include "TransferCurveAnalyzer.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>

std::vector<RunConfig> buildRunGrid(const Config& config, const std::vector<juce::String>& paramNames) {
    std::vector<RunConfig> runs;

    // Convert ParameterBucketConfig to BucketSpec and generate values
    std::vector<std::pair<juce::String, std::vector<float>>> paramValueLists;

    for (const auto& bucketConfig : config.parameterBuckets) {
        BucketSpec spec;
        spec.paramName = bucketConfig.paramName;
        spec.strategy = BucketSpec::strategyFromString(bucketConfig.strategy);
        spec.min = bucketConfig.min;
        spec.max = bucketConfig.max;
        spec.numBuckets = bucketConfig.numBuckets;
        spec.values = bucketConfig.values;

        auto values = spec.generateValues();
        paramValueLists.push_back({bucketConfig.paramName, values});
    }

    // Build Cartesian product of parameter values and input gain buckets
    int runId = 0;

    // Helper function to generate combinations recursively
    std::function<void(int, std::map<juce::String, float>)> generateCombinations;
    generateCombinations = [&](int paramIndex, std::map<juce::String, float> currentParams) {
        if (paramIndex >= (int)paramValueLists.size()) {
            // All parameters set, now combine with input gain buckets
            for (float inputGainDb : config.inputGainBucketsDb) {
                RunConfig run;
                run.runId = runId++;
                run.paramValues = currentParams;
                run.inputGainDb = inputGainDb;
                runs.push_back(run);
            }
            return;
        }

        // Try all values for current parameter
        const auto& [paramName, values] = paramValueLists[paramIndex];
        for (float value : values) {
            auto newParams = currentParams;
            newParams[paramName] = value;
            generateCombinations(paramIndex + 1, newParams);
        }
    };

    generateCombinations(0, {});

    return runs;
}

std::vector<std::unique_ptr<Analyzer>> createAnalyzers(const Config& config, const juce::File& outDir,
                                                       const std::vector<juce::String>& paramNames) {
    std::vector<std::unique_ptr<Analyzer>> analyzers;

    for (const auto& analyzerName : config.analyzers) {
        if (analyzerName.equalsIgnoreCase("RawCsv")) {
            analyzers.push_back(createRawCsvAnalyzer(outDir));
        } else if (analyzerName.equalsIgnoreCase("RmsPeak")) {
            analyzers.push_back(createRmsPeakAnalyzer(outDir, paramNames));
        } else if (analyzerName.equalsIgnoreCase("TransferCurve")) {
            analyzers.push_back(createTransferCurveAnalyzer(outDir, 512, paramNames));
        } else if (analyzerName.equalsIgnoreCase("LinearResponse")) {
            if (config.signalType.equalsIgnoreCase("noise") || config.signalType.equalsIgnoreCase("sweep")) {
                analyzers.push_back(createLinearResponseAnalyzer(outDir, 4096, paramNames));
            } else {
                std::cerr << "Warning: LinearResponse analyzer requires noise or sweep signal type" << std::endl;
            }
        } else if (analyzerName.equalsIgnoreCase("Thd")) {
            if (config.signalType.equalsIgnoreCase("sine")) {
                analyzers.push_back(createThdAnalyzer(outDir, 2048, config.sineFrequency, paramNames));
            } else {
                std::cerr << "Warning: Thd analyzer requires sine signal type" << std::endl;
            }
        } else {
            std::cerr << "Warning: Unknown analyzer: " << analyzerName << std::endl;
        }
    }

    return analyzers;
}

void runMeasurementGrid(juce::AudioPluginInstance& plugin, double sampleRate, int blockSize, int64 totalSamples,
                        const std::vector<RunConfig>& runs, const std::vector<std::unique_ptr<Analyzer>>& analyzers,
                        const Config& config, const juce::File& outDir) {
    auto paramMap = buildParameterMap(plugin);

    // Build parameter name list in order
    std::vector<juce::String> paramNames;
    for (const auto& bucket : config.parameterBuckets) {
        paramNames.push_back(bucket.paramName);
    }

    juce::AudioBuffer<float> inputBuffer(2, blockSize);
    juce::AudioBuffer<float> outputBuffer(2, blockSize);
    juce::MidiBuffer midiBuffer;

    for (const auto& run : runs) {
        std::cout << "Running measurement " << run.runId << " / " << runs.size() << std::endl;

        // Set plugin parameters
        for (const auto& [paramName, value] : run.paramValues) {
            setParameterValue(plugin, paramMap, paramName, value);
        }

        // Convert input gain from dB to linear amplitude
        float inputGainLinear = std::pow(10.0f, run.inputGainDb / 20.0f);

        // Create signal generator
        std::unique_ptr<SineGenerator> sineGen;
        std::unique_ptr<NoiseGenerator> noiseGen;
        std::unique_ptr<SweepGenerator> sweepGen;

        if (config.signalType.equalsIgnoreCase("sine")) {
            sineGen = std::make_unique<SineGenerator>();
            sineGen->sampleRate = sampleRate;
            sineGen->frequency = config.sineFrequency;
            sineGen->amplitude = inputGainLinear;
        } else if (config.signalType.equalsIgnoreCase("noise")) {
            noiseGen = std::make_unique<NoiseGenerator>();
            noiseGen->amplitude = inputGainLinear;
        } else if (config.signalType.equalsIgnoreCase("sweep")) {
            sweepGen = std::make_unique<SweepGenerator>();
            sweepGen->sampleRate = sampleRate;
            sweepGen->startHz = config.sweepStartHz;
            sweepGen->endHz = config.sweepEndHz;
            sweepGen->duration = config.seconds;
            sweepGen->amplitude = inputGainLinear;
            sweepGen->reset();
        }

        // Process samples
        int64 currentSample = 0;
        while (currentSample < totalSamples) {
            int numThisBlock = (int)std::min((int64)blockSize, totalSamples - currentSample);

            // Clear buffers
            inputBuffer.clear();
            outputBuffer.clear();

            // Fill input with test signal
            if (sineGen) {
                sineGen->fillBlock(inputBuffer, numThisBlock);
            } else if (noiseGen) {
                noiseGen->fillBlock(inputBuffer, numThisBlock);
            } else if (sweepGen) {
                sweepGen->fillBlock(inputBuffer, numThisBlock);
            }

            // Process through plugin
            plugin.processBlock(inputBuffer, outputBuffer, midiBuffer);

            // Build BlockContext
            BlockContext ctx;
            ctx.firstSample = currentSample;
            ctx.sampleRate = sampleRate;
            ctx.numSamples = numThisBlock;
            ctx.inL = inputBuffer.getReadPointer(0);
            ctx.inR = inputBuffer.getNumChannels() > 1 ? inputBuffer.getReadPointer(1) : nullptr;
            ctx.outL = outputBuffer.getReadPointer(0);
            ctx.outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getReadPointer(1) : nullptr;
            ctx.runId = run.runId;
            ctx.paramNamedValues = run.paramValues;
            ctx.inputGainDb = run.inputGainDb;

            // Build params vector in fixed order
            for (const auto& paramName : paramNames) {
                float value = 0.0f;
                auto it = run.paramValues.find(paramName);
                if (it != run.paramValues.end())
                    value = it->second;
                ctx.params.push_back(value);
            }

            // Process through analyzers
            for (auto& analyzer : analyzers) {
                analyzer->processBlock(ctx);
            }

            currentSample += numThisBlock;
        }
    }

    // Finish all analyzers
    for (auto& analyzer : analyzers) {
        analyzer->finish(outDir);
    }
}
