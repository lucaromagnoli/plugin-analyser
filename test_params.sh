#!/bin/bash
PLUGIN="/Volumes/External SSD/Plug-Ins/VST3/Acustica/GAINSTATION2.vst3"
echo "Testing plugin: $PLUGIN"
if [ ! -d "$PLUGIN" ]; then
    echo "Plugin not found!"
    exit 1
fi

# Create a minimal config just to test loading
cat > /tmp/test_minimal_config.json << 'CONFIG'
{
  "pluginPath": "",
  "sampleRate": 48000,
  "seconds": 0.1,
  "blockSize": 256,
  "signalType": "sine",
  "sineFrequency": 1000,
  "inputGainBucketsDb": [0],
  "parameterBuckets": [],
  "analyzers": ["RmsPeak"]
}
CONFIG

# Use the CLI tool to load and see what happens
./build-release/plugin_measure_grid_cli --plugin "$PLUGIN" --config /tmp/test_minimal_config.json --out /tmp/test_output 2>&1 | head -50
