/*
Author: Thomas Daley
Date: September 8, 2016

Usage: Chip8 filename [-FLSV]
    -F      Launch in fullscreen
    -L      Enable load/store quirk
    -S      Enable shift quirk
    -V      Disable vertical wrapping
*/

#include "Chip8.h"
#include <string.h>

#ifdef __APPLE__
/* There is an extra check here for launching Cocoa based .app's by 
   double clicking on them or (equivalently) using the "open" command. 
   On MacOS, GUI apps launch with an extra argument that gets sent in 
   the form of -psn_X_XXXXX where X can be any number. This is the 
   Process Serial Number. */
#define ROM_PATH_EXISTS (argc >= 2 && strncmp(argv[1], "-psn", 4))
#endif

#ifdef _WIN32
#define ROM_PATH_EXISTS (argc >= 2)
#include "../Windows/src/resource.h" /* window icon */
#endif

int main(int argc, char **argv){
    
    /* Defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 0;
    bool shift_quirk = 0;
    bool vwrap = 1;

    /* Parse and set any options present */
    for (int i = 2; i < argc; i++){

        char *pos = argv[i];
        if (*pos == '-') {
            
            pos++;
            int len = strlen(pos);

            for (int j = 0; j < len; j++) {

                if (*pos == 'F') fullscreen = 1;
                if (*pos == 'L') load_store_quirk = 1;
                if (*pos == 'S') shift_quirk = 1;
                if (*pos == 'V') vwrap = 0;
                pos++;

            }
        }
    }

    Chip8 chip = Chip8();
    if (chip.Initialize(fullscreen, load_store_quirk, shift_quirk, vwrap)) {
        return 1;
    }

    /* Load ROM from argument vector */
    if (ROM_PATH_EXISTS) {
        if (chip.Load(argv[1])) return 1;  
    } 

    /* Where the magic happens */
    chip.Run();
    return 0;
}
