#include "chip8.h"
#include <string.h>

#ifdef _WIN32
#include "../windows/src/resource.h" /* window icon */
#endif

int main(int argc, char **argv){

    /* defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 1;
    bool shift_quirk = 1;
    bool vwrap = 1;
    bool muted = 0;

    chip8_create();

    /* parse and set any options present */
    for (int i = 1; i < argc; i++){

        char *pos = argv[i];
        if (*pos == '-') {

            pos++;
            int len = strlen(pos);

            for (int j = 0; j < len; j++) {

                if (*pos == 'F') fullscreen = 1;
                if (*pos == 'M') muted = 1;
                if (*pos == 'L') load_store_quirk = 0;
                if (*pos == 'S') shift_quirk = 0;
                if (*pos == 'V') vwrap = 0;
                pos++;
            }
        }
    }

    /* calling initialize() also loads the bootrom */
    if (chip8_initialize(
        fullscreen,
        load_store_quirk,
        shift_quirk,
        vwrap,
        muted
    )) return 1;

    /* load ROM from argument vector */
    if (argc >= 2 && *argv[1] != '-') {
        if (chip8_load(argv[1])) return 1;
    }

    chip8_run();
    chip8_destroy();
    return 0;
}
