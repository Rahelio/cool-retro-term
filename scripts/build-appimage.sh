#!/usr/bin/env bash
set -euo pipefail
set -x

REPO_ROOT="$(readlink -f "$(dirname "$(dirname "$0")")")"
OLD_CWD="$(readlink -f .)"
BUILD_DIR="$REPO_ROOT/build/appimage"
TOOLS_DIR="$BUILD_DIR/tools"
VERSION="$(git -C "$REPO_ROOT" describe --tags --always --dirty=-dirty 2>/dev/null || true)"
if [ -z "$VERSION" ]; then
    VERSION="unknown"
fi

if ! command -v cmake >/dev/null; then
    echo "cmake not found in PATH." >&2
    exit 1
fi

APPDIR="$BUILD_DIR/AppDir"
CMAKE_BUILD_DIR="$BUILD_DIR/cmake"

mkdir -p "$BUILD_DIR"
rm -rf "$APPDIR"

CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr)
if [ -n "${QT_ROOT_DIR:-}" ]; then
    CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=$QT_ROOT_DIR")
fi

cmake -S "$REPO_ROOT" -B "$CMAKE_BUILD_DIR" "${CMAKE_ARGS[@]}"
cmake --build "$CMAKE_BUILD_DIR" -j"$(nproc)"
DESTDIR="$APPDIR" cmake --install "$CMAKE_BUILD_DIR"

# Relocate QMLTermWidget into the standard AppDir QML import path.
QMLTERMWIDGET_DIR="$(find "$APPDIR" -type d -name QMLTermWidget -print -quit)"
if [ -n "$QMLTERMWIDGET_DIR" ] && [ "$QMLTERMWIDGET_DIR" != "$APPDIR/usr/qml/QMLTermWidget" ]; then
    mkdir -p "$APPDIR/usr/qml"
    rsync -a "$QMLTERMWIDGET_DIR" "$APPDIR/usr/qml/"
    rm -rf "$QMLTERMWIDGET_DIR"
    QML_PARENT="$(dirname "$QMLTERMWIDGET_DIR")"
    while [ "$QML_PARENT" != "$APPDIR" ] && rmdir "$QML_PARENT" 2>/dev/null; do
        QML_PARENT="$(dirname "$QML_PARENT")"
    done
fi


# Build AppImage using linuxdeploy and linuxdeploy-plugin-qt.
mkdir -p "$TOOLS_DIR"
LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT="$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"
if [ ! -x "$LINUXDEPLOY" ]; then
    wget -c -O "$LINUXDEPLOY" https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x "$LINUXDEPLOY"
fi
if [ ! -x "$LINUXDEPLOY_QT" ]; then
    wget -c -O "$LINUXDEPLOY_QT" https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x "$LINUXDEPLOY_QT"
fi

export QML_SOURCES_PATHS="$REPO_ROOT/app"

pushd "$BUILD_DIR"

export EXTRA_PLATFORM_PLUGINS="libqwayland.so;"
export EXTRA_QT_MODULES="sql;waylandcompositor;waylandclient"
export DEPLOY_PLATFORM_THEMES=true
export LINUXDEPLOY_EXCLUDED_LIBRARIES="libmysqlclient.so;libqsqlmimer.so;libqsqlmysql.so;libqsqlodbc.so;libqsqlpsql.so;libqsqloci.so;libqsqlibase.so"

"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    -e "$APPDIR/usr/bin/cool-retro-term" \
    -i "$REPO_ROOT/app/icons/256x256/cool-retro-term.png" \
    -d "$REPO_ROOT/cool-retro-term.desktop" \
    --plugin qt \
    --output appimage

APPIMAGE_PATH="$(ls -1 ./*.AppImage | head -n 1)"
APPIMAGE_OUT="cool-retro-term-${VERSION}.AppImage"
mv "$APPIMAGE_PATH" "$OLD_CWD/$APPIMAGE_OUT"
popd
