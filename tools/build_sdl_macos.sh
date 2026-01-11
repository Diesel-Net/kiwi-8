#!/usr/bin/env bash
set -euo pipefail

# build_sdl_macos.sh
# Builds SDL 2.32.10 as a universal macOS binary and installs it into MacOS/frameworks/sdl

SDL_VERSION="2.32.10"
SDL_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL2-${SDL_VERSION}.tar.gz"
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
TP_DIR="$ROOT_DIR/MacOS/third_party"
BUILD_DIR="$TP_DIR/SDL2-${SDL_VERSION}/build-universal"
INSTALL_DIR="$ROOT_DIR/MacOS/frameworks/sdl"

mkdir -p "$TP_DIR"
cd "$TP_DIR"

# download if needed
if [ ! -f "SDL2-${SDL_VERSION}.tar.gz" ]; then
  echo "Downloading SDL ${SDL_VERSION}..."
  curl -L -o "SDL2-${SDL_VERSION}.tar.gz" "$SDL_URL"
fi

# extract
if [ ! -d "SDL2-${SDL_VERSION}" ]; then
  tar xzf "SDL2-${SDL_VERSION}.tar.gz"
fi

# configure & build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=ON
cmake --build . -- -j $(sysctl -n hw.ncpu)

# backup
ts=$(date +%Y%m%d%H%M%S)
mkdir -p "$INSTALL_DIR/backups"
cp -a "$INSTALL_DIR/lib" "$INSTALL_DIR/backups/lib.$ts"
cp -a "$INSTALL_DIR/include" "$INSTALL_DIR/backups/include.$ts"

# copy artifacts
cp "$BUILD_DIR/libSDL2-2.0.0.dylib" "$INSTALL_DIR/lib/libSDL2-2.0.0.dylib"
ln -sf libSDL2-2.0.0.dylib "$INSTALL_DIR/lib/libSDL2-2.0.dylib"
ln -sf libSDL2-2.0.0.dylib "$INSTALL_DIR/lib/libSDL2.dylib"
cp "$BUILD_DIR/libSDL2.a" "$INSTALL_DIR/lib/libSDL2.a"
cp "$BUILD_DIR/libSDL2_test.a" "$INSTALL_DIR/lib/libSDL2_test.a"
cp "$BUILD_DIR/libSDL2main.a" "$INSTALL_DIR/lib/libSDL2main.a"

# install headers
rm -rf "$INSTALL_DIR/include/SDL2"
cp -a "$BUILD_DIR/include/SDL2" "$INSTALL_DIR/include/"
cp -a "$BUILD_DIR/include-config-release/SDL2/SDL_config.h" "$INSTALL_DIR/include/SDL2/SDL_config.h"

echo "SDL ${SDL_VERSION} built and installed into ${INSTALL_DIR}"