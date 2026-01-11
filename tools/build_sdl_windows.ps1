# build_sdl_windows.ps1
# Builds SDL 2.32.10 for Windows x64 and installs it into Windows/frameworks/sdl

param(
    [string]$SDL_VERSION = "2.32.10",
    [string]$ROOT_DIR = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
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
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON -DSDL_STATIC=ON
cmake --build . --config Release -- -m

# Backup
$timestamp = Get-Date -Format "yyyyMMddHHmmss"
$BACKUP_DIR = "$INSTALL_DIR/backups"
New-Item -ItemType Directory -Force -Path $BACKUP_DIR | Out-Null
if (Test-Path "$INSTALL_DIR/lib") {
    Copy-Item -Path "$INSTALL_DIR/lib" -Destination "$BACKUP_DIR/lib.$timestamp" -Recurse -Force
}
if (Test-Path "$INSTALL_DIR/include") {
    Copy-Item -Path "$INSTALL_DIR/include" -Destination "$BACKUP_DIR/include.$timestamp" -Recurse -Force
}

# Create install directories
New-Item -ItemType Directory -Force -Path "$INSTALL_DIR/lib/x64" | Out-Null
New-Item -ItemType Directory -Force -Path "$INSTALL_DIR/include" | Out-Null

# Copy artifacts (Release build)
$RELEASE_BUILD = "Release"
Copy-Item -Path "$BUILD_DIR/$RELEASE_BUILD/SDL2.lib" -Destination "$INSTALL_DIR/lib/x64/SDL2.lib" -Force
Copy-Item -Path "$BUILD_DIR/$RELEASE_BUILD/SDL2.dll" -Destination "$INSTALL_DIR/lib/x64/SDL2.dll" -Force
Copy-Item -Path "$BUILD_DIR/$RELEASE_BUILD/SDL2main.lib" -Destination "$INSTALL_DIR/lib/x64/SDL2main.lib" -Force

# Install headers (SDL2 source headers are in include/, not include/SDL2/)
Remove-Item -Path "$INSTALL_DIR/include/SDL2" -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path "$INSTALL_DIR/include/SDL2" | Out-Null
Copy-Item -Path "$EXTRACT_DIR_ABS/include/*" -Destination "$INSTALL_DIR/include/SDL2/" -Recurse -Force

# Find and copy SDL_config.h (generated during build, location varies)
$CONFIG_H = $null
Write-Host "Looking for SDL_config.h..."
Write-Host "  Checking: $BUILD_DIR/$RELEASE_BUILD/SDL_config.h"
Write-Host "  Checking: $BUILD_DIR/SDL_config.h"
Write-Host "  Checking: $EXTRACT_DIR_ABS/include/SDL_config.h"

# Try common CMake-generated locations
$CANDIDATES = @(
    "$BUILD_DIR/$RELEASE_BUILD/SDL_config.h",
    "$BUILD_DIR/SDL_config.h",
    "$BUILD_DIR/include/SDL_config.h",
    "$EXTRACT_DIR_ABS/include/SDL_config.h"
)

foreach ($candidate in $CANDIDATES) {
    if (Test-Path $candidate) {
        Write-Host "  Found at: $candidate"
        $CONFIG_H = $candidate
        break
    }
}

if ($CONFIG_H) {
    Copy-Item -Path $CONFIG_H -Destination "$INSTALL_DIR/include/SDL2/SDL_config.h" -Force
    Write-Host "SDL_config.h copied from $CONFIG_H"
} else {
    Write-Host "SDL_config.h not found. Searching build directory..."
    Get-ChildItem -Path $BUILD_DIR -Filter "SDL_config.h" -Recurse | ForEach-Object {
        Write-Host "  Found: $($_.FullName)"
    }
}

Write-Host "SDL ${SDL_VERSION} built and installed into ${INSTALL_DIR}"
Pop-Location
Pop-Location
