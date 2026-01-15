cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================

project(Kiwi8 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Version Detection (from env, git tag, or git branch)
# ============================================================================

if(DEFINED ENV{GITHUB_REF_NAME})
    set(PROJECT_VERSION_STRING "$ENV{GITHUB_REF_NAME}")
else()
    # Try git describe (exact tag match)
    execute_process(
        COMMAND git describe --tags --exact-match
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(GIT_VERSION)
        set(PROJECT_VERSION_STRING "${GIT_VERSION}")
    else()
        # Fallback to git branch
        execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(GIT_BRANCH)
            set(PROJECT_VERSION_STRING "${GIT_BRANCH}")
        else()
            set(PROJECT_VERSION_STRING "unknown")
        endif()
    endif()
endif()

set(APP_NAME_VERSION "Kiwi8 ${PROJECT_VERSION_STRING}")

# ============================================================================
# Generated Headers
# ============================================================================

set(LICENSE_HEADER "${CMAKE_BINARY_DIR}/generated/license.h")
set(BOOTROM_HEADER "${CMAKE_BINARY_DIR}/generated/bootrom.h")
set(BOOTROM_SOURCE "${CMAKE_SOURCE_DIR}/roms/Kiwi8_logo_2.ch8")

# Create generated directory
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")

# Generate license header
add_custom_command(
    OUTPUT "${LICENSE_HEADER}"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/tools/generate_license_header.py"
            "${CMAKE_SOURCE_DIR}/LICENSE"
            "${LICENSE_HEADER}"
    DEPENDS "${CMAKE_SOURCE_DIR}/LICENSE"
    COMMENT "Generating license header"
)

# Generate bootrom header
add_custom_command(
    OUTPUT "${BOOTROM_HEADER}"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/tools/generate_bootrom_header.py"
            "${BOOTROM_SOURCE}"
            "${BOOTROM_HEADER}"
    DEPENDS "${BOOTROM_SOURCE}"
    COMMENT "Generating bootrom header"
)

# ============================================================================
# SDL2 Dependency
# ============================================================================

set(SDL_BUILD_DIR "${CMAKE_SOURCE_DIR}/external/sdl/build")
set(SDL_INCLUDE_DIR "${SDL_BUILD_DIR}/include")
set(SDL_LIB_DIR "${SDL_BUILD_DIR}/lib")

# Check if SDL is built
if(NOT EXISTS "${SDL_INCLUDE_DIR}/SDL2/SDL.h" OR NOT EXISTS "${SDL_LIB_DIR}")
    if(APPLE)
        message(STATUS "SDL2 not found. Building SDL2 for macOS...")
        execute_process(
            COMMAND bash "${CMAKE_SOURCE_DIR}/tools/build_sdl_macos.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE SDL_BUILD_RESULT
        )
        if(NOT SDL_BUILD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to build SDL2")
        endif()
    elseif(WIN32)
        message(STATUS "SDL2 not found. Building SDL2 for Windows...")
        execute_process(
            COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/tools/build_sdl_windows.ps1"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            RESULT_VARIABLE SDL_BUILD_RESULT
        )
        if(NOT SDL_BUILD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to build SDL2")
        endif()
    endif()
endif()

# Add SDL2 as an imported library
if(APPLE)
    set(SDL_LIB_NAME libSDL2-2.0.0.dylib)
else()
    set(SDL_LIB_NAME SDL2.lib)
endif()

add_library(SDL2 SHARED IMPORTED GLOBAL)
set_target_properties(SDL2 PROPERTIES
    IMPORTED_LOCATION "${SDL_LIB_DIR}/${SDL_LIB_NAME}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_INCLUDE_DIR}"
)

# ============================================================================
# Source Files
# ============================================================================

set(CORE_SRCS
    shared/Audio.cc
    shared/Chip8.cc
    shared/Display.cc
    shared/Gui.cc
    shared/imgui_impl_sdl.cc
    shared/Input.cc
    shared/main.cc
    shared/opcodes.cc
    shared/open_file_dialog.cc
)

set(IMGUI_SRCS
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
)

if(APPLE)
    set(PLATFORM_SRCS
        macos/src/file_dialog.mm
    )
    set(PLATFORM_RESOURCES)
    # Files that depend on generated headers
    set_source_files_properties(
        shared/Gui.cc
        PROPERTIES
        OBJECT_DEPENDS "${LICENSE_HEADER}"
    )
    set_source_files_properties(
        shared/Chip8.cc
        PROPERTIES
        OBJECT_DEPENDS "${BOOTROM_HEADER}"
    )
elseif(WIN32)
    set(PLATFORM_SRCS
        windows/src/file_dialog.cc
    )
    # RC resource file for Windows icon and metadata
    set(RC_FILE "${CMAKE_SOURCE_DIR}/windows/src/Kiwi8.rc")
    set(RC_COMPILED "${CMAKE_BINARY_DIR}/Kiwi8.res")

    # Compile RC file to resource object file
    add_custom_command(
        OUTPUT "${RC_COMPILED}"
        COMMAND RC "${RC_FILE}" /fo "${RC_COMPILED}"
        DEPENDS "${RC_FILE}" "${CMAKE_SOURCE_DIR}/windows/resources/Kiwi8.ico" "${CMAKE_SOURCE_DIR}/windows/src/resource.h"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Compiling resource file"
    )

    list(APPEND PLATFORM_SRCS "${RC_COMPILED}")
    set(PLATFORM_RESOURCES "${RC_COMPILED}")

    # Files that depend on generated headers
    set_source_files_properties(
        shared/Gui.cc
        PROPERTIES
        OBJECT_DEPENDS "${LICENSE_HEADER}"
    )
    set_source_files_properties(
        shared/Chip8.cc
        PROPERTIES
        OBJECT_DEPENDS "${BOOTROM_HEADER}"
    )
endif()

# ============================================================================
# Compiler Configuration
# ============================================================================

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
    add_compile_options(-Wall)
    add_compile_definitions(APPNAME_VERSION="${APP_NAME_VERSION}")
elseif(WIN32)
    add_compile_options(/W4 /w44996)
    add_compile_definitions(APPNAME_VERSION="${APP_NAME_VERSION}")
endif()

# ============================================================================
# Debug Target
# ============================================================================

add_executable(Kiwi8_debug
    ${CORE_SRCS}
    ${IMGUI_SRCS}
    ${PLATFORM_SRCS}
    "${LICENSE_HEADER}"
    "${BOOTROM_HEADER}"
)

target_include_directories(Kiwi8_debug PRIVATE
    "${SDL_INCLUDE_DIR}"
    external/imgui
    shared
    "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(Kiwi8_debug PRIVATE SDL2)

if(APPLE)
    target_link_libraries(Kiwi8_debug PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
    )
    target_compile_options(Kiwi8_debug PRIVATE -g)
    target_link_options(Kiwi8_debug PRIVATE -mmacosx-version-min=11.0)

    # Set output directory
    set_target_properties(Kiwi8_debug PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/debug"
        OUTPUT_NAME "Kiwi8"
    )

    # Custom command to fix dylib and copy SDL
    add_custom_command(TARGET Kiwi8_debug POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL_LIB_DIR}/${SDL_LIB_NAME}"
                "$<TARGET_FILE_DIR:Kiwi8_debug>/libSDL2-2.0.0.dylib"
        COMMAND install_name_tool -id @executable_path/libSDL2-2.0.0.dylib
                "$<TARGET_FILE_DIR:Kiwi8_debug>/libSDL2-2.0.0.dylib"
        COMMAND install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/libSDL2-2.0.0.dylib
                "$<TARGET_FILE:Kiwi8_debug>"
        COMMENT "Setting up dylib paths for debug"
    )
elseif(WIN32)
    target_compile_options(Kiwi8_debug PRIVATE /Zi /MD)
    target_link_options(Kiwi8_debug PRIVATE /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup)

    set_target_properties(Kiwi8_debug PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/debug"
    )

    add_custom_command(TARGET Kiwi8_debug POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL_LIB_DIR}/SDL2.dll"
                "$<TARGET_FILE_DIR:Kiwi8_debug>/SDL2.dll"
        COMMENT "Copying SDL2 DLL to debug"
    )
endif()

# ============================================================================
# Release Target
# ============================================================================

add_executable(Kiwi8_release
    ${CORE_SRCS}
    ${IMGUI_SRCS}
    ${PLATFORM_SRCS}
    "${LICENSE_HEADER}"
    "${BOOTROM_HEADER}"
)

target_include_directories(Kiwi8_release PRIVATE
    "${SDL_INCLUDE_DIR}"
    external/imgui
    shared
    "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(Kiwi8_release PRIVATE SDL2)

if(APPLE)
    target_link_libraries(Kiwi8_release PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
    )
    target_link_options(Kiwi8_release PRIVATE -mmacosx-version-min=11.0)

    set(APP_BUNDLE "${CMAKE_BINARY_DIR}/release/Kiwi8.app")
    set_target_properties(Kiwi8_release PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release/Kiwi8.app/Contents/MacOS"
        OUTPUT_NAME "Kiwi8"
    )

    add_custom_command(TARGET Kiwi8_release POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${APP_BUNDLE}/Contents/Resources"
                "${APP_BUNDLE}/Contents/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL_LIB_DIR}/${SDL_LIB_NAME}"
                "${APP_BUNDLE}/Contents/Frameworks/libSDL2-2.0.0.dylib"
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_SOURCE_DIR}/macos/resources/Kiwi8.icns"
                "${APP_BUNDLE}/Contents/Resources/Kiwi8.icns"
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_SOURCE_DIR}/macos/src/Info.plist"
                "${APP_BUNDLE}/Contents/Info.plist"
        COMMAND install_name_tool -id @executable_path/../Frameworks/libSDL2-2.0.0.dylib
                "${APP_BUNDLE}/Contents/Frameworks/libSDL2-2.0.0.dylib"
        COMMAND install_name_tool -change @rpath/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib
                "$<TARGET_FILE:Kiwi8_release>"
        COMMENT "Creating macOS app bundle"
    )
elseif(WIN32)
    target_compile_options(Kiwi8_release PRIVATE /MD)
    target_link_options(Kiwi8_release PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)

    set_target_properties(Kiwi8_release PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release"
    )

    add_custom_command(TARGET Kiwi8_release POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL_LIB_DIR}/SDL2.dll"
                "$<TARGET_FILE_DIR:Kiwi8_release>/SDL2.dll"
        COMMENT "Copying SDL2 DLL to release"
    )
endif()

# ============================================================================
# Custom Targets
# ============================================================================

add_custom_target(all_targets DEPENDS Kiwi8_debug Kiwi8_release)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/LICENSE"
    "${BOOTROM_SOURCE}"
)

# SDL rebuild target
add_custom_target(sdl
    COMMAND bash "${CMAKE_SOURCE_DIR}/tools/build_sdl_macos.sh"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Building SDL2"
)

# Run targets
if(APPLE)
    add_custom_target(run-debug
        COMMAND "${CMAKE_BINARY_DIR}/debug/Kiwi8"
        DEPENDS Kiwi8_debug
        COMMENT "Running debug executable"
    )

    add_custom_target(run-release
        COMMAND open "${CMAKE_BINARY_DIR}/release/Kiwi8.app"
        DEPENDS Kiwi8_release
        COMMENT "Running release app bundle"
    )
elseif(WIN32)
    add_custom_target(run-debug
        COMMAND "${CMAKE_BINARY_DIR}/debug/Kiwi8.exe"
        DEPENDS Kiwi8_debug
        COMMENT "Running debug executable"
    )

    add_custom_target(run-release
        COMMAND "${CMAKE_BINARY_DIR}/release/Kiwi8.exe"
        DEPENDS Kiwi8_release
        COMMENT "Running release executable"
    )
endif()

# Clean targets (mirrors makefile behavior)

# Regular clean: removes build artifacts but keeps SDL (like: make clean)
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}"
    COMMENT "Clean: removing build artifacts (keeping SDL)"
)

# Distclean: removes everything including SDL (like: make distclean)
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/external/sdl"
    COMMENT "Distclean: removing build artifacts and SDL"
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "========== Kiwi8 Build Configuration ==========")
message(STATUS "Version: ${APP_NAME_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(APPLE)
    message(STATUS "Architectures: ${CMAKE_OSX_ARCHITECTURES}")
    message(STATUS "Min macOS: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()
message(STATUS "Build directories:")
message(STATUS "  - Debug: ${CMAKE_BINARY_DIR}/debug")
message(STATUS "  - Release: ${CMAKE_BINARY_DIR}/release")
message(STATUS "Build with: cmake --build ${CMAKE_BINARY_DIR}")
message(STATUS "=============================================")
