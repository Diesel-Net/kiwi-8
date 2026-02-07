#ifndef USAGE_H
#define USAGE_H

/* Centralized usage/help text for CLI and GUI */
static const char *USAGE_TEXT =
    "Usage: Kiwi8 [options] [rom_file]\n"
    "Options:\n"
    "  -f, --fullscreen         Start in fullscreen\n"
    "  -m, --muted              Start with audio muted\n"
    "  -p, --profiles <path>    Use a custom profiles.ini file\n"
    "  -h, --help               Show this help message\n"
    "\n"
    "Note: Quirks are configured per-ROM via profiles.ini or GUI.\n";

#endif /* USAGE_H */
