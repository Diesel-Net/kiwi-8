#include "chip8.h"
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
    struct quirks quirks = {
        1, // load_store_quirk
        1, // shift_quirk
        0, // jump_quirk
        0, // logic_vf_quirk
        0, // i_overflow_quirk
        0, // draw_flag_quirk
        1, // vwrap
        0  // hwrap
    };

    /* parse and set any options present */
    for (int i = 1; i < argc; i++){
        char *pos = argv[i];
        if (*pos == '-') {
            pos++;
            int len = strlen(pos);
            for (int j = 0; j < len; j++) {
                if (*pos == 'F') fullscreen = 1;
                if (*pos == 'M') muted = 1;
                if (*pos == 'L') quirks.load_store_quirk = 0;
                if (*pos == 'S') quirks.shift_quirk = 0;
                if (*pos == 'V') quirks.vwrap = 0;
                if (*pos == 'H') quirks.hwrap = 1;
                if (*pos == 'J') quirks.jump_quirk = 1;
                if (*pos == 'G') quirks.logic_vf_quirk = 1;
                if (*pos == 'I') quirks.i_overflow_quirk = 1;
                if (*pos == 'D') quirks.draw_flag_quirk = 1;
                pos++;
            }
        }
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "%s", USAGE_TEXT);
            return 0;
        }
    }

    /* calling initialize() also loads the bootrom */
    if (chip8_initialize(
        fullscreen,
        quirks,
        muted
    )) return 1;

    /* load ROM from the last non-option argument */
    if (argc > 1 && argv[argc-1][0] != '-') {
        if (chip8_load(argv[argc-1])) return 1;
    }

    chip8_run();
    chip8_destroy();
    return 0;
}
