#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="/Users/jeff/build-adblink"

cpack --config "$BUILD_DIR/CPackConfig.cmake" -B "$BUILD_DIR/packages"
ls -lh "$BUILD_DIR/packages/"
