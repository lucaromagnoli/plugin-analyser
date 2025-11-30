.PHONY: help build build-debug build-release clean test format check-format install run run-gui run-cli

# Default build directory
BUILD_DIR := build
BUILD_DEBUG := build-debug
BUILD_RELEASE := build-release

# Default target
help:
	@echo "Plugin Analyser - Available commands:"
	@echo ""
	@echo "Build:"
	@echo "  build              - Configure and build in Debug mode"
	@echo "  build-debug        - Build in Debug mode"
	@echo "  build-release      - Build in Release mode"
	@echo ""
	@echo "Run:"
	@echo "  run-gui            - Build and run GUI application"
	@echo "  run-cli            - Build and run CLI tool (requires config)"
	@echo ""
	@echo "Code Quality:"
	@echo "  format             - Format all C++ code with clang-format"
	@echo "  check-format       - Check code formatting (no changes)"
	@echo ""
	@echo "Utilities:"
	@echo "  clean              - Clean build directories"
	@echo "  install            - Install pre-commit hooks"

# Configure and build Debug
build: build-debug

build-debug:
	@echo "🔨 Configuring CMake (Debug)..."
	@mkdir -p $(BUILD_DEBUG)
	@cmake -S . -B $(BUILD_DEBUG) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@echo "🔨 Building (Debug)..."
	@cmake --build $(BUILD_DEBUG) --parallel
	@echo "✅ Build complete! Executables in $(BUILD_DEBUG)/"

# Build Release
build-release:
	@echo "🔨 Configuring CMake (Release)..."
	@mkdir -p $(BUILD_RELEASE)
	@cmake -S . -B $(BUILD_RELEASE) -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@echo "🔨 Building (Release)..."
	@cmake --build $(BUILD_RELEASE) --parallel
	@echo "✅ Build complete! Executables in $(BUILD_RELEASE)/"

# Clean build directories
clean:
	@echo "🧹 Cleaning build directories..."
	@rm -rf $(BUILD_DIR) $(BUILD_DEBUG) $(BUILD_RELEASE)
	@rm -f compile_commands.json
	@echo "✅ Clean complete!"

# Format code
format:
	@echo "🎨 Formatting code with clang-format..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find src -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +; \
		echo "✅ Formatting complete!"; \
	else \
		echo "⚠️  clang-format not found, skipping formatting"; \
	fi

# Check formatting
check-format:
	@echo "🔍 Checking code formatting..."
	@if command -v clang-format >/dev/null 2>&1; then \
		FILES=$$(find src -type f \( -name "*.cpp" -o -name "*.h" \)); \
		MISMATCH=0; \
		for file in $$FILES; do \
			if ! clang-format "$$file" | diff -q "$$file" - >/dev/null 2>&1; then \
				echo "❌ Formatting issue in: $$file"; \
				MISMATCH=1; \
			fi; \
		done; \
		if [ $$MISMATCH -eq 0 ]; then \
			echo "✅ All files are properly formatted!"; \
		else \
			echo "❌ Some files need formatting. Run 'make format' to fix."; \
			exit 1; \
		fi; \
	else \
		echo "⚠️  clang-format not found, skipping check"; \
	fi

# Install pre-commit hooks
install:
	@echo "📦 Installing pre-commit hooks..."
	@if command -v pre-commit >/dev/null 2>&1; then \
		pre-commit install; \
		echo "✅ Pre-commit hooks installed!"; \
	else \
		echo "⚠️  pre-commit not found. Install with: pip install pre-commit"; \
	fi

# Run GUI application
run-gui: build-release
	@echo "🚀 Running GUI application..."
	@$(BUILD_RELEASE)/PluginAnalyser.app/Contents/MacOS/PluginAnalyser 2>/dev/null || \
	 $(BUILD_RELEASE)/PluginAnalyser 2>/dev/null || \
	 echo "⚠️  Executable not found. Check build output."

# Run CLI tool (example)
run-cli: build-release
	@echo "🚀 Running CLI tool..."
	@echo "⚠️  Usage: $(BUILD_RELEASE)/plugin_measure_grid_cli --config example_config.json --out ./output"
	@if [ -f example_config.json ]; then \
		$(BUILD_RELEASE)/plugin_measure_grid_cli --config example_config.json --out ./output || \
		echo "⚠️  Executable not found. Check build output."; \
	else \
		echo "⚠️  example_config.json not found. Create a config file first."; \
	fi

