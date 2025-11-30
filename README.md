# Plugin Analyser

A C++ tool (JUCE-based) for measuring VST3 audio plugins with Plugin Doctor-style analysis. Available as both a **GUI application** and a **command-line tool**.

## Features

- Load and test VST3 plugins offline
- Configurable parameter grid testing
- Multiple analysis modes:
  - Oscilloscope-style time-domain output
  - Linear frequency response
  - Harmonic distortion / THD
  - Static transfer curve
  - Static dynamics (input→output level)
- Export data as CSV for Python analysis

## Building

### Prerequisites

- CMake 3.22+
- C++17 compatible compiler
- JUCE 8.0+ (automatically fetched via CMake)

### Build Steps

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Two executables will be built:
- `PluginAnalyser` - GUI application (recommended)
- `plugin_measure_grid_cli` - Command-line tool

## Usage

### GUI Application (Recommended)

Simply run `PluginAnalyser` and use the graphical interface to:
1. Browse and load a VST3 plugin
2. View all available plugin parameters
3. Select which parameters to measure
4. Configure bucket strategies for each parameter
5. Configure measurement settings (signal type, analyzers, etc.)
6. Run measurements and view progress
7. Export results to CSV

### Command-Line Tool

```bash
plugin_measure_grid_cli --config config.json --out /path/to/output
```

### Command Line Options

- `--config <path>`: JSON configuration file (required)
- `--out <path>`: Output directory (required, created if needed)
- `--plugin <path>`: Override pluginPath in JSON
- `--seconds N`: Override duration in seconds
- `--samplerate SR`: Override sample rate
- `--blocksize BS`: Override block size

## Configuration

See `example_config.json` for a complete example configuration file.

## Analyzers

The tool supports the following analyzers:

- **RawCsv**: Exports raw time-domain samples (oscilloscope-style)
- **RmsPeak**: Computes RMS and peak levels for input/output (static dynamics)
- **TransferCurve**: Maps input→output relationship (useful for Hammerstein modeling)
- **LinearResponse**: Frequency response from noise or sweep signals
- **Thd**: Total Harmonic Distortion analysis for sine signals

## Signal Types

- **sine**: Pure sine wave at specified frequency (for THD analysis)
- **noise**: White noise (for frequency response)
- **sweep**: Logarithmic frequency sweep (for frequency response)

## Parameter Bucket Strategies

- **Linear**: Evenly spaced values between min and max
- **ExplicitValues**: Use exact values from the `values` array
- **Log**: Logarithmically spaced values
- **EdgeAndCenter**: Three points: min, center, max

## Example Output

The tool generates CSV files in the output directory:

- `raw.csv`: Time-domain samples (if RawCsv analyzer enabled)
- `grid_rms_peak.csv`: RMS and peak measurements per run
- `grid_transfer_curves.csv`: Input→output transfer curves
- `grid_linear_response.csv`: Frequency response (if LinearResponse enabled)
- `grid_thd.csv`: THD measurements (if Thd analyzer enabled)

## License

MIT License - see LICENSE file for details.

