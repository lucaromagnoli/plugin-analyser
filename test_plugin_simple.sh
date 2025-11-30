#!/bin/bash
PLUGIN="/Users/lucaromagnoli/Library/Audio/Plug-Ins/VST3/MusicalAideas.vst3"
echo "Testing plugin: $PLUGIN"
echo "Plugin exists: $([ -d "$PLUGIN" ] && echo "yes" || echo "no")"
echo ""
echo "Running CLI tool..."
./build-release/plugin_measure_grid_cli --plugin "$PLUGIN" --config example_config.json --out /tmp/test_output 2>&1
