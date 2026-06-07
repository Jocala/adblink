#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="/Volumes/source/adblink"
BUILD_DIR="/Users/jeff/build-adblink"

cmake -S "$SOURCE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure
cpack --config "$BUILD_DIR/CPackConfig.cmake" -B "$BUILD_DIR/packages"
ls -lh "$BUILD_DIR/packages/"
