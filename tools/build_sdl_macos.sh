#!/usr/bin/env bash
set -euo pipefail

# Downloads SDL source to external/sdl and builds it as a universal macOS binary
# Installs into external/sdl/build

SDL_VERSION="2.32.10"
SDL_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL2-${SDL_VERSION}.tar.gz"
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SDL_SRC_DIR="$ROOT_DIR/external/sdl"
BUILD_DIR="$SDL_SRC_DIR/SDL2-${SDL_VERSION}/build-universal"
INSTALL_DIR="$SDL_SRC_DIR/build"

mkdir -p "$SDL_SRC_DIR"
cd "$SDL_SRC_DIR"

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
