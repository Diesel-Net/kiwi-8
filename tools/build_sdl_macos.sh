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
# Allow overriding the deployment target via SDL_DEPLOYMENT_TARGET env var (defaults to 11.0)
SDL_DEPLOYMENT_TARGET="${SDL_DEPLOYMENT_TARGET:-11.0}"
echo "Building SDL with CMAKE_OSX_DEPLOYMENT_TARGET=${SDL_DEPLOYMENT_TARGET}"
cmake .. -DCMAKE_OSX_DEPLOYMENT_TARGET="${SDL_DEPLOYMENT_TARGET}" -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=ON -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
cmake --build . --config Release --parallel

# install using CMake (handles all files correctly)
echo "Installing SDL ${SDL_VERSION}..."
cmake --install . --config Release
