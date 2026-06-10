#!/usr/bin/env bash
# Stage a release: build all platforms, stage packages, stage website edits.
# Run from macOS. Does NOT deploy to production (jocala.com).
# Usage: ./stage.sh

set -euo pipefail

SCRIPT_DIR="$(dirname "$0")"
VERSION=$(grep 'const QString version' "$SCRIPT_DIR/version.h" | sed 's/.*"\(.*\)".*/\1/')

DEBIAN="jeff@192.168.1.39"
WINDOWS="jeff@192.168.1.137"
DOWNLOADS="/zstore/source/www/jocala.com/downloads"
WEBSITE="/zstore/source/www/jocala.com"
ADBLINK_REPO="/zstore/source/adblink"

DMG="adblink-${VERSION}-Darwin.dmg"
TGZ="adblink-${VERSION}-Linux.tar.gz"
EXE="adblink-${VERSION}-win64.exe"

echo "=== Staging v${VERSION} ==="
echo ""

# -- Build & package -------------------------------------------------------

echo "--- Building macOS ---"
cd "$(dirname "$0")"
./build-adblink-macos.sh
./package-adblink-macos.sh

echo ""
echo "--- Building Linux ---"
ssh "$DEBIAN" "cd $ADBLINK_REPO && git stash && git pull && chmod +x *.sh && ./build-adblink-linux.sh && ./package-adblink-linux.sh"

echo ""
echo "--- Building Windows ---"
ssh "$WINDOWS" "cd C:\\source\\adblink && git pull && git config core.filemode false"
ssh "$WINDOWS" "powershell -ExecutionPolicy Bypass -File C:\\source\\adblink\\build-adblink-windows.ps1"
ssh "$WINDOWS" "powershell -ExecutionPolicy Bypass -File C:\\source\\adblink\\package-adblink-windows.ps1"

# -- Stage packages --------------------------------------------------------

echo ""
echo "--- Copying packages to Debian downloads directory ---"

scp "/Users/jeff/build-adblink/packages/$DMG" "$DEBIAN:$DOWNLOADS/$DMG"
ssh "$DEBIAN" "cp /home/jeff/build-adblink/packages/$TGZ $DOWNLOADS/$TGZ"

# Copy Windows package via SSH from Windows to Debian
ssh "$WINDOWS" "scp C:\\Users\\jeff\\build-adblink\\packages\\$EXE ${DEBIAN}:${DOWNLOADS}/${EXE}"

# -- Verify ----------------------------------------------------------------

echo ""
echo "--- Verifying staged packages ---"
ssh "$DEBIAN" "ls -lh $DOWNLOADS/$DMG $DOWNLOADS/$TGZ $DOWNLOADS/$EXE"

# -- Update website files --------------------------------------------------

echo ""
echo "--- Updating version.txt and index.html ---"

OLDVER=$(ssh "$DEBIAN" "cat $WEBSITE/version.txt" | xargs)
echo "Current version: $OLDVER, new version: $VERSION"

ssh "$DEBIAN" "echo '$VERSION' > $WEBSITE/version.txt"

# Replace download filenames in index.html (any old version → new)
ssh "$DEBIAN" "sed -i 's|downloads/adblink-[0-9.]*-Darwin.dmg|downloads/$DMG|g' $WEBSITE/index.html"
ssh "$DEBIAN" "sed -i 's|downloads/adblink-[0-9.]*-Linux.tar.gz|downloads/$TGZ|g' $WEBSITE/index.html"
ssh "$DEBIAN" "sed -i 's|downloads/adblink-[0-9.]*-win64.exe|downloads/$EXE|g' $WEBSITE/index.html"

# Replace version labels in link text (any old version → new)
ssh "$DEBIAN" "sed -i '/for \(Windows\|macOS\|Linux\)/s/v[0-9.]*/v${VERSION}/g' $WEBSITE/index.html"

# -- Remind about remaining manual steps -----------------------------------

echo ""
echo "=== Staging complete ==="
echo "Packages staged at $DEBIAN:$DOWNLOADS"
echo "version.txt and index.html updated."
echo ""
echo "Remaining steps (on Debian):"
echo "  1. Edit $WEBSITE/changelog.txt        -> add release notes"
echo "  2. cd $WEBSITE && git add -A && git commit -m \"v${VERSION} release\""
echo ""
echo "When ready to go live (from Debian):"
echo "  cd $ADBLINK_REPO"
echo "  ./deploy.sh all $DOWNLOADS/$DMG $DOWNLOADS/$TGZ $DOWNLOADS/$EXE"
echo "  cd $WEBSITE && git push"
