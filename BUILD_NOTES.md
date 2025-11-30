# Build Notes

## Dependencies

- CMake 3.22+
- C++17 compiler
- JUCE 8.0+ (automatically fetched via CMake FetchContent)

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Known Issues / TODO

1. **JUCE FFT API**: The FFT implementation in `LinearResponseAnalyzer` and `ThdAnalyzer` uses `juce::dsp::FFT`. The API may need adjustment based on your JUCE version. The `perform()` method signature might differ.

2. **Plugin Loading**: The plugin loading code uses `findAllTypesForFile()` which should work, but you may need to adjust based on your JUCE version.

3. **Parameter Matching**: Parameter names are matched case-insensitively after trimming. Some plugins may have non-standard parameter names that need manual mapping.

4. **FFT Size**: The FFT sizes are hardcoded (4096 for LinearResponse, 2048 for Thd). These could be made configurable.

5. **Window Function**: The Hann window implementation assumes N-1 in the denominator. For N=1, this will cause division by zero - add a guard if needed.

## Testing

After building, test with:

```bash
./plugin_measure_grid --config ../example_config.json --out ./test_output
```

Make sure to update the `pluginPath` in `example_config.json` to point to an actual VST3 plugin.

