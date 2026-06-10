#!/usr/bin/env bash
set -euo pipefail

CMAKE="/opt/homebrew/bin/cmake"
CTEST="/opt/homebrew/bin/ctest"

SOURCE_DIR="/Users/jeff/source/adblink"
BUILD_DIR="/Users/jeff/build-adblink"

$CMAKE -S "$SOURCE_DIR" -B "$BUILD_DIR" -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
$CMAKE --build "$BUILD_DIR"
$CTEST --test-dir "$BUILD_DIR" --output-on-failure
