/*
Author: Thomas Daley
Date: September 8, 2016

Usage: Kiwi8 [filename] [-FMLSV]
    -F      Launch in fullscreen
    -M      Launch with audio muted
    -L      Disable load/store quirk
    -S      Disable shift quirk
    -V      Disable vertical wrapping
*/

#include "Chip8.h"
#include <string.h>

#ifdef _WIN32
#include "../Windows/src/resource.h" /* window icon */
#endif

int main(int argc, char **argv){
    
    /* Defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 1;
    bool shift_quirk = 1;
    bool vwrap = 1;
    bool muted = 0;

    /* Parse and set any options present */
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

    Chip8 chip = Chip8();
    if (chip.Initialize(fullscreen, load_store_quirk, shift_quirk, vwrap, muted)) {
        return 1;
    }

    /* Load ROM from argument vector */
    if (argc >= 2 && *argv[1] != '-') {
        if (chip.Load(argv[1])) return 1;  
    } 

    /* Where the magic happens */
    chip.Run();
    return 0;
}
