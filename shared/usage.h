#ifndef USAGE_H
#define USAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "meta.h"

/* Centralized usage/help text for CLI and GUI */
static const char *USAGE_TEXT =
    "Usage: " APP_NAME " [options] [rom_file]\n"
    "Options:\n"
    "  -f, --fullscreen         Start in fullscreen\n"
    "  -m, --muted              Start with audio muted\n"
    "  -p, --profiles <path>    Use a custom profiles.ini file\n"
    "  -h, --help               Show this help message\n"
    "\n"
    "Note: Quirks are configured per-ROM via profiles.ini or GUI.\n";

#ifdef __cplusplus
}
#endif

#endif /* USAGE_H */
