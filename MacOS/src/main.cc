/*
Author: Thomas Daley
Date: September 8, 2016

on MacOS: 
    open ./Chip8.app --args "/ABSOLUTE_PATH_TO_ROM.ch8" -F 255 255 255
*/

#include "../../Core/Chip8.h"

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

    
    /* Short circuit trick needed when double-clicking .app's or 
       launching with the "open" command. */
    if (argc < 2 || !strncmp(argv[1], "-psn", 4)) {
        
        /* Load default ROM */
        if (chip.LoadDefault()) return 1;
        
    } else {

        /* Load ROM from argument vector */
        if (chip.Load(argv[1])) return 1;
    }
    
    chip.Run();
    return 0;
}
