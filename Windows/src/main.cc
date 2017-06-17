/*
Author: Thomas Daley
Date: September 8, 2016
*/

#include "../../Core/Chip8.h"
#include "resource.h"
#include "fileDialog.h"
#include <windows.h>
#include <string.h>

int main(int argc, char **argv){
    
    char rom_name[MAX_PATH];

    /* Defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 0;
    bool shift_quirk = 0;
    bool vwrap = 1;
    unsigned char R = 200;
    unsigned char G = 170;
    unsigned char B = 255;

    if (argc < 2) {

        /* Open file dialogue */
        openFileDialog(rom_name, "Chip8\0*.ch8\0All\0*.*\0");
        
    } else {
        strcpy(rom_name, argv[1]);
    }


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
    
    if (chip.Load(rom_name)) {
        return 1;
    }
    
    chip.Run();
    return 0;
}
