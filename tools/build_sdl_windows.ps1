# Downloads SDL source to external/sdl and builds it as a 64-bit Windows binary
# Installs into external/sdl/build

param(
    [string]$SDL_VERSION = "2.32.10",
    [string]$ROOT_DIR = (Split-Path -Parent $PSScriptRoot)
)

$SDL_URL = "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL2-${SDL_VERSION}.tar.gz"
$SDL_SRC_DIR = "$ROOT_DIR/external/sdl"
$INSTALL_DIR = "$SDL_SRC_DIR/build"

# Create external/sdl directory
New-Item -ItemType Directory -Force -Path $SDL_SRC_DIR | Out-Null
Push-Location $SDL_SRC_DIR

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
    if (-Not (Get-Command tar -ErrorAction SilentlyContinue)) {
        Write-Error "tar is required to extract SDL. Please install Windows 10+ or enable tar support."
        exit 1
    }
    tar -xzf $ARCHIVE
}

# Convert to absolute path before changing directory
# First ensure the build directory exists
$BUILD_DIR_PATH = "$SDL_SRC_DIR/$EXTRACT_DIR/build-x64"
New-Item -ItemType Directory -Force -Path $BUILD_DIR_PATH | Out-Null
$BUILD_DIR = (Resolve-Path $BUILD_DIR_PATH).Path

# Configure & build
Push-Location $BUILD_DIR

Write-Host "Building SDL for x64..."
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=ON -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
cmake --build . --config Release --parallel

# Install using CMake (handles all headers including SDL_config.h)
Write-Host "Installing SDL artifacts..."
cmake --install . --config Release
