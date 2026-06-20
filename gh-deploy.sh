#!/usr/bin/env bash
# Create a GitHub release with all 3 platform installers as assets.
# Run from macOS after staging is complete.
# Prerequisites: gh CLI authenticated (run `gh auth login --hostname github.com --git-protocol ssh` once)
set -euo pipefail

SCRIPT_DIR="$(dirname "$0")"
VERSION=$(grep 'const QString version' "$SCRIPT_DIR/version.h" | sed 's/.*"\(.*\)".*/\1/')

DEBIAN="jeff@192.168.1.39"
DOWNLOADS="/zstore/source/www/jocala.com/downloads"
BUILD_DIR="/Users/jeff/build-adblink/packages"
TMPDIR="/tmp/adblink-gh-release"

DMG="adblink-${VERSION}-Darwin.dmg"
TGZ="adblink-${VERSION}-Linux.tar.gz"
EXE="adblink-${VERSION}-win64.exe"
TAG="v${VERSION}"

BREW="/opt/homebrew/bin/brew"
GH="/opt/homebrew/bin/gh"

if ! test -x "$GH"; then
  echo "Installing gh CLI via Homebrew..."
  $BREW install gh
fi

echo "Checking gh auth status..."
if ! "$GH" auth status &>/dev/null; then
  echo "gh is not authenticated. Run:"
  echo "  $GH auth login --hostname github.com --git-protocol ssh"
  exit 1
fi

echo ""
echo "=== Creating GitHub release ${TAG} ==="

echo "Pushing source to GitHub..."
git push github main
git push github --force --tags

echo "Gathering installer assets..."
mkdir -p "$TMPDIR"

if [ ! -f "$BUILD_DIR/$DMG" ]; then
  echo "ERROR: macOS package not found at $BUILD_DIR/$DMG"
  exit 1
fi

echo "Copying Linux and Windows packages from Debian..."
scp "$DEBIAN:$DOWNLOADS/$TGZ" "$TMPDIR/$TGZ"
scp "$DEBIAN:$DOWNLOADS/$EXE" "$TMPDIR/$EXE"

echo "Deleting existing release (if any)..."
"$GH" release delete "$TAG" --yes 2>/dev/null || true

echo "Creating release..."
"$GH" release create "$TAG" \
  "$BUILD_DIR/$DMG" \
  "$TMPDIR/$TGZ" \
  "$TMPDIR/$EXE" \
  --title "adblink ${VERSION}" \
  --notes "Installers for macOS, Linux, and Windows. See https://jocala.com/changelog.txt for full release notes."

echo "Cleaning up..."
rm -rf "$TMPDIR"

echo ""
echo "=== Done ==="
echo "Release ${TAG} created at: https://github.com/Jocala/adblink/releases/tag/${TAG}"
