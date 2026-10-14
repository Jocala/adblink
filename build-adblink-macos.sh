#!/usr/bin/env bash
set -euo pipefail

CMAKE="/opt/homebrew/bin/cmake"
CTEST="/opt/homebrew/bin/ctest"

SOURCE_DIR="/Users/jeff/source/adblink"
BUILD_DIR="/Users/jeff/source/builds/adblink"

# Xcode 26.4 dropped x86_64 from cdefs.h, breaking universal builds. Xcode 26.6+ restored it.
# If the universal build breaks again, uncomment the sysroot pin:
#   -DCMAKE_OSX_SYSROOT="/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk"
$CMAKE -S "$SOURCE_DIR" -B "$BUILD_DIR" \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="14.0"
$CMAKE --build "$BUILD_DIR"
# Sign the dev bundle with the release identity so TCC/SMB grants survive
# rebuilds (ad-hoc linker signatures churn every build). Same flags as
# packaging/sign-after-install.cmake.
/usr/bin/codesign --force --deep --sign \
  "Developer ID Application: jeff elkins (9Q77WK7W3R)" \
  --timestamp --options=runtime \
  --entitlements "$SOURCE_DIR/packaging/adblink.entitlements" \
  "$BUILD_DIR/adblink.app"
$CTEST --test-dir "$BUILD_DIR" --output-on-failure
