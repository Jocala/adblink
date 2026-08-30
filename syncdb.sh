#!/usr/bin/env bash
# Copy ~/.jocala/adblink.db from macOS to the Linux and Windows machines.
set -euo pipefail

DB="$HOME/.jocala/adblink.db"
DEBIAN="jeff@192.168.1.39"
# Windows hosts — easy switch:
#   ./syncdb.sh                    # uses primary (170)
#   WINDOWS_HOST=backup ./syncdb.sh  # uses backup (137)
WINDOWS_PRIMARY="jeff@192.168.1.170"   # win10 VM — primary
WINDOWS_BACKUP="jeff@192.168.1.137"    # win11 — backup
if [ "${WINDOWS_HOST:-}" = "backup" ]; then
  WINDOWS="$WINDOWS_BACKUP"
elif [ -n "${WINDOWS_HOST:-}" ]; then
  WINDOWS="$WINDOWS_HOST"
else
  WINDOWS="$WINDOWS_PRIMARY"
fi
LINUX_PATH="/home/jeff/.jocala/adblink.db"
WINDOWS_PATH="C:/Users/jeff/AppData/Roaming/.jocala/adblink.db"

if [ ! -f "$DB" ]; then
  echo "ERROR: source database not found at $DB"
  exit 1
fi

if pgrep -f "adblink.app/Contents/MacOS/adblink" >/dev/null 2>&1; then
  echo "WARNING: adblink is running on macOS; database copy may be inconsistent"
fi

LOCAL_SIZE=$(wc -c < "$DB" | tr -d ' ')
echo "Copying $DB ($LOCAL_SIZE bytes)"

scp "$DB" "$DEBIAN:$LINUX_PATH"
echo "Copied to $DEBIAN:$LINUX_PATH"

scp "$DB" "$WINDOWS:$WINDOWS_PATH"
echo "Copied to $WINDOWS:$WINDOWS_PATH"

LINUX_SIZE=$(ssh "$DEBIAN" "wc -c < $LINUX_PATH" | tr -d ' ')
WINDOWS_SIZE=$(ssh "$WINDOWS" "(Get-Item '$WINDOWS_PATH').Length" | tr -d '\r')

echo "Verification:"
echo "  macOS:   $LOCAL_SIZE bytes"
echo "  Linux:   $LINUX_SIZE bytes"
echo "  Windows: $WINDOWS_SIZE bytes"

if [ "$LOCAL_SIZE" = "$LINUX_SIZE" ] && [ "$LOCAL_SIZE" = "$WINDOWS_SIZE" ]; then
  echo "OK: all three databases match"
else
  echo "ERROR: database sizes do not match"
  exit 1
fi
