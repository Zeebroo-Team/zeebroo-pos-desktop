#!/usr/bin/env bash
# Package ZeebrooPosDesktop for distribution (macOS, Linux, Windows).
set -euo pipefail

PLATFORM="${1:?Usage: package-desktop.sh <macos|linux|windows>}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
POS_DIR="$REPO_ROOT/pos-desktop"
BUILD_DIR="$POS_DIR/build"
DIST_DIR="$POS_DIR/dist"
APP_NAME="ZeebrooPosDesktop"

VERSION="$(sed -n 's/project(ZeebrooPosDesktop VERSION \([0-9.]*\).*/\1/p' "$POS_DIR/CMakeLists.txt")"
VERSION="${VERSION:-0.0.0}"

if [[ -z "${QT_ROOT_DIR:-}" ]]; then
  echo "QT_ROOT_DIR is not set. Install Qt before packaging." >&2
  exit 1
fi

export PATH="${QT_ROOT_DIR}/bin:${PATH}"

rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

cd "$POS_DIR"

case "$PLATFORM" in
  macos)
    APP_BUNDLE="$BUILD_DIR/${APP_NAME}.app"
    if [[ ! -d "$APP_BUNDLE" ]]; then
      APP_BUNDLE="$(find "$BUILD_DIR" -maxdepth 4 -type d -name "${APP_NAME}.app" 2>/dev/null | head -1 || true)"
    fi
    if [[ -z "$APP_BUNDLE" || ! -d "$APP_BUNDLE" ]]; then
      echo "Missing app bundle under $BUILD_DIR (expected ${APP_NAME}.app)" >&2
      find "$BUILD_DIR" -maxdepth 4 \( -name "${APP_NAME}" -o -name "${APP_NAME}.app" \) -print 2>/dev/null || true
      exit 1
    fi
    macdeployqt "$APP_BUNDLE" -always-overwrite
    mkdir -p "$DIST_DIR/package"
    cp -R "$APP_BUNDLE" "$DIST_DIR/package/"
    cp config.example.json "$DIST_DIR/package/"
    ARCHIVE="$DIST_DIR/${APP_NAME}-${VERSION}-macos.zip"
    (cd "$DIST_DIR/package" && zip -r "$ARCHIVE" .)
    ;;

  linux)
    BINARY="$BUILD_DIR/${APP_NAME}"
    if [[ ! -f "$BINARY" ]]; then
      echo "Missing binary: $BINARY" >&2
      exit 1
    fi
    LINUXDEPLOYQT="$DIST_DIR/linuxdeployqt"
    if [[ ! -x "$LINUXDEPLOYQT" ]]; then
      curl -fsSL -o "$LINUXDEPLOYQT" \
        "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
      chmod +x "$LINUXDEPLOYQT"
    fi
    # linuxdeployqt must run on glibc <= 2.35 (e.g. Ubuntu 22.04) — see issue #340.
    export APPIMAGE_EXTRACT_AND_RUN=1
    mkdir -p "$DIST_DIR/package"
    cp "$BINARY" "$DIST_DIR/package/"
    cp config.example.json "$DIST_DIR/package/"
    "$LINUXDEPLOYQT" "$DIST_DIR/package/${APP_NAME}" -bundle-non-qt-libs
    ARCHIVE="$DIST_DIR/${APP_NAME}-${VERSION}-linux-x86_64.tar.gz"
    tar -czf "$ARCHIVE" -C "$DIST_DIR/package" .
    ;;

  windows)
    EXE="$BUILD_DIR/${APP_NAME}.exe"
    if [[ ! -f "$EXE" ]]; then
      echo "Missing executable: $EXE" >&2
      exit 1
    fi
    mkdir -p "$DIST_DIR/package"
    cp "$EXE" "$DIST_DIR/package/"
    windeployqt "$DIST_DIR/package/${APP_NAME}.exe" --release --compiler-runtime
    cp config.example.json "$DIST_DIR/package/"
    ARCHIVE="$DIST_DIR/${APP_NAME}-${VERSION}-windows-x64.zip"
    # Use tar (not PowerShell): Git Bash paths like /d/a/... break Compress-Archive.
    (cd "$DIST_DIR/package" && tar -a -c -f "$ARCHIVE" .)
    ;;

  *)
    echo "Unknown platform: $PLATFORM" >&2
    exit 1
    ;;
esac

echo "Packaged: $DIST_DIR"
