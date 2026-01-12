# build_sdl_windows.ps1
# Builds SDL 2.32.10 for Windows x64 and installs it into Windows/frameworks/sdl

param(
    [string]$SDL_VERSION = "2.32.10",
    [string]$ROOT_DIR = (Split-Path -Parent $PSScriptRoot)
)

$SDL_URL = "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL2-${SDL_VERSION}.tar.gz"
$TP_DIR = "$ROOT_DIR/Windows/third_party"
$INSTALL_DIR = "$ROOT_DIR/Windows/frameworks/sdl"

# Create third_party directory
New-Item -ItemType Directory -Force -Path $TP_DIR | Out-Null
Push-Location $TP_DIR

# Download if needed
$ARCHIVE = "SDL2-${SDL_VERSION}.tar.gz"
if (-Not (Test-Path $ARCHIVE)) {
    Write-Host "Downloading SDL ${SDL_VERSION}..."
    Invoke-WebRequest -Uri $SDL_URL -OutFile $ARCHIVE
}

# Extract if needed
$EXTRACT_DIR = "SDL2-${SDL_VERSION}"
if (-Not (Test-Path $EXTRACT_DIR)) {
    Write-Host "Extracting SDL..."
    # Use tar directly (simpler for .tar.gz)
    if (Get-Command tar -ErrorAction SilentlyContinue) {
        tar -xzf $ARCHIVE
    } else {
        # Fallback: 7z requires two-step extraction for .tar.gz
        7z x $ARCHIVE
        7z x "SDL2-${SDL_VERSION}.tar"
    }
}

# Convert to absolute path before changing directory
# First ensure the build directory exists
$BUILD_DIR_PATH = "$TP_DIR/$EXTRACT_DIR/build-x64"
New-Item -ItemType Directory -Force -Path $BUILD_DIR_PATH | Out-Null
$BUILD_DIR = (Resolve-Path $BUILD_DIR_PATH).Path
$EXTRACT_DIR_ABS = (Resolve-Path "$TP_DIR/$EXTRACT_DIR").Path

# Configure & build
Push-Location $BUILD_DIR

Write-Host "Building SDL for x64..."
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=ON -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
cmake --build . --config Release --parallel

# Install using CMake (handles all headers including SDL_config.h)
Write-Host "Installing SDL artifacts..."
cmake --install . --config Release
