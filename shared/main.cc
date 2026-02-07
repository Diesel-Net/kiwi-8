#include "chip8.h"
#include "quirks.h"
#include "usage.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include "../windows/src/resource.h" /* window icon */
#endif

int main(int argc, char **argv){

    /* defaults */
    bool fullscreen = 0;
    bool muted = 0;
    const char *profiles_path = NULL;

    /* parse and set any options present */
    for (int i = 1; i < argc; i++){
        char *pos = argv[i];
        if (*pos == '-') {
            pos++;
            int len = strlen(pos);
            for (int j = 0; j < len; j++) {
                if (*pos == 'f') fullscreen = 1;
                if (*pos == 'm') muted = 1;
                if (*pos == 'p' && i + 1 < argc) {
                    profiles_path = argv[++i];
                }
                pos++;
            }
        }
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("%s", USAGE_TEXT);
            return 0;
        }
        if (strcmp(argv[i], "--fullscreen") == 0) fullscreen = 1;
        if (strcmp(argv[i], "--muted") == 0) muted = 1;
        if (strcmp(argv[i], "--profiles") == 0 && i + 1 < argc) {
            profiles_path = argv[++i];
        }
    }

    /* calling init() also loads the bootrom */
    if (chip8_init(
        fullscreen,
        muted,
        profiles_path
    )) return 1;

    /* load ROM from the last non-option argument */
    if (argc > 1 && argv[argc-1][0] != '-') {
        if (chip8_load_rom(argv[argc-1])) return 1;
    }

    chip8_run();
    chip8_destroy();
    return 0;
}
