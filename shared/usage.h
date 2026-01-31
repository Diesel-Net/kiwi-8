#ifndef USAGE_H
#define USAGE_H

/* Centralized usage/help text for CLI and GUI */
static const char *USAGE_TEXT =
    "Usage: Kiwi8 [options] [rom]\n"
    "Options:\n"
    "  -F    Fullscreen\n"
    "  -M    Mute audio\n"
    "  -L    Disable load/store quirk\n"
    "  -S    Disable shift quirk\n"
    "  -V    Disable vertical wrapping\n"
    "  -H    Enable horizontal wrapping (DXYN)\n"
    "  -J    Enable jump with VX offset (BNNN)\n"
    "  -G    Enable logic ops VF=0 (8XY1/2/3)\n"
    "  -I    Enable I+VX overflow quirk (FX1E)\n"
    "  -D    Enable draw flag reset quirk\n"
    "  -h, --help  Show this help message\n";

#endif /* USAGE_H */
