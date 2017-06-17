/*
Author: Thomas Daley
Date: September 8, 2016
*/

#include "Chip8.h"
#include <string.h>

#ifdef __APPLE__
/* This is a short circuit trick for launching Cocoa based .app's by 
   double clicking on them or (equivalently) using the "open" command. 
   On MacOS, GUI apps launch with an extra argument that gets sent in 
   the form of -psn_X_XXXXX where X can be any number. This is the 
   Process Serial Number. */
#define GUI_LAUNCH_CONDITION (argc < 2 || !strncmp(argv[1], "-psn", 4))
#endif

#ifdef _WIN32
#define GUI_LAUNCH_CONDITION (argc < 2)
#include <windows.h> /* atoi() */
#include "../Windows/src/resource.h" /* window icon */
#endif

int main(int argc, char **argv){
    
    /* Defaults */
    Defaults def;
    bool fullscreen = def.fullscreen;
    bool load_store_quirk = def.load_store_quirk;
    bool shift_quirk = def.shift_quirk;
    bool vwrap = def.vwrap;
    unsigned char R = def.R;
    unsigned char G = def.G;
    unsigned char B = def.B;

    if (argc >= 3) {

        char *pos = argv[2];
        if (*pos == '-') {
            
            pos++;
            int len = strlen(pos);

            for (int i = 0; i < len; i++) {
                /* Parse the options */
                if (*pos == 'F') fullscreen = 1;
                if (*pos == 'L') load_store_quirk = 1;
                if (*pos == 'S') shift_quirk = 1;
                if (*pos == 'V') vwrap = 0;
                pos++;
            }

        } else {
            /* No options, just color */
            if (argc >= 5) {
                R = (unsigned char) atoi(argv[2]);
                G = (unsigned char) atoi(argv[3]);
                B = (unsigned char) atoi(argv[4]);
            }
        }
    }

    if (argc >= 6) {
        /* Options and color */
        R = (unsigned char) atoi(argv[3]);
        G = (unsigned char) atoi(argv[4]);
        B = (unsigned char) atoi(argv[5]);   
    }

    Chip8 chip = Chip8();
    if (chip.Initialize(fullscreen, load_store_quirk, shift_quirk, vwrap, R, G, B)) {
        return 1;
    }

    if (GUI_LAUNCH_CONDITION) {
        
        /* Load default ROM */
        if (chip.LoadDefault()) return 1;
        
    } else {

        /* Load ROM from argument vector */
        if (chip.Load(argv[1])) return 1;
    }

    /* Where the magic happens */
    return chip.Run();
}
