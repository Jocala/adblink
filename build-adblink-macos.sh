#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="/Users/jeff/source/adblink"
BUILD_DIR="/Users/jeff/build-adblink"

cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure
