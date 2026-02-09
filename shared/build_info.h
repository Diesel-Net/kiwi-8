#ifndef BUILD_INFO_H
#define BUILD_INFO_H

/* Detect OS at compile time */
#if defined(_WIN32) || defined(_WIN64)
    #define BUILD_OS "Windows"
#elif defined(__APPLE__) || defined(__MACH__)
    #define BUILD_OS "macOS"
#elif defined(__linux__)
    #define BUILD_OS "Linux"
#else
    #define BUILD_OS "unknown"
#endif

/* Windows icon resource */
#ifdef _WIN32
#include "../windows/src/resource.h"
#endif

/* Detect architecture at compile time */
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    #define BUILD_ARCH "ARM64"
#elif defined(__x86_64__) || defined(_M_X64)
    #define BUILD_ARCH "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
    #define BUILD_ARCH "x86"
#else
    #define BUILD_ARCH "unknown"
#endif

// APP_NAME is defined by the compiler via -DAPP_NAME="..."
#ifndef APP_NAME
#define APP_NAME "Kiwi8"
#endif

// VERSION is defined by the compiler via -DVERSION="..."
#ifndef VERSION
#define VERSION "develop"
#endif

// SUB_VERSION is defined by the compiler via -DSUB_VERSION="..."
#ifndef SUB_VERSION
#define SUB_VERSION "unknown"
#endif

#endif
