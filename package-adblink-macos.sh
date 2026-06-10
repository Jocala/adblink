#!/usr/bin/env bash
set -euo pipefail

CPACK="/opt/homebrew/bin/cpack"

BUILD_DIR="/Users/jeff/build-adblink"

$CPACK --config "$BUILD_DIR/CPackConfig.cmake" -B "$BUILD_DIR/packages"
ls -lh "$BUILD_DIR/packages/"
