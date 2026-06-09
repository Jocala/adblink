#!/bin/sh
# Deploy website HTML and/or builds to production
# Usage: ./deploy.sh {html|builds <files>|all <files>}

set -e

RSYNC_OPTS="-avz"
PROD="jeff@jocala.com:/var/www/jocala.com/public_html"
WWW="/zstore/source/www/jocala.com"

case "${1:-html}" in
  html)
    echo "Deploying HTML + version.txt..."
    rsync $RSYNC_OPTS \
      --include='*.html' \
      --include='version.txt' \
      --include='*/' \
      --exclude='*' \
      "$WWW/" "$PROD/"
    ;;
  builds)
    shift
    if [ $# -eq 0 ]; then
      echo "Usage: $0 builds <file1> [file2 ...]"
      exit 1
    fi
    echo "Deploying builds..."
    rsync $RSYNC_OPTS "$@" "$PROD/downloads/"
    ;;
  all)
    shift
    echo "Deploying HTML..."
    rsync $RSYNC_OPTS \
      --include='*.html' \
      --include='version.txt' \
      --include='*/' \
      --exclude='*' \
      "$WWW/" "$PROD/"
    echo "Deploying builds..."
    rsync $RSYNC_OPTS "$@" "$PROD/downloads/"
    ;;
  *)
    echo "Usage: $0 {html|builds <files>|all <files>}"
    exit 1
    ;;
esac
