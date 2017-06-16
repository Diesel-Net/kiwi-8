/*
Author: Thomas Daley
Date: September 8, 2016

on MacOS: 
    open ./Chip8.app --args "/ABSOLUTE_PATH_TO_ROM.ch8" -F 255 255 255
*/

#include "../../Core/Chip8.h"
#include "fileDialog.h"

int main(int argc, char **argv){

    char rom_name[PATH_MAX];
    memset(rom_name, 0 , PATH_MAX);

    /* Defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 0;
    bool shift_quirk = 0;
    bool vwrap = 1;
    unsigned char R = 200;
    unsigned char G = 170;
    unsigned char B = 255;

    if (argc < 2 || argv[1][0] == '-') {

        /* NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@VERSION];
        [alert setInformativeText:@"Click OK to select a ROM file.\n\n"
                                "Alternatively, you may launch Chip8 with the command line.\n\n"
                                "Usage: Chip8 PATH_TO_ROM [-FLS] [R G B]\n"
                                "       -F\t\tLaunch in fullscreen\n"
                                "       -L\t\tEnable load/store quirk\n"
                                "       -S\t\tEnable shift quirk\n"
                                "       -V\t\tDisable vertical wrapping\n"
                                "       R G B\t\tForeground color in RGB format,\n"
                                "            \t\t3 numbers from 0-255\n\n\n\n"
                                "Enjoy!\n\nThomas Daley"];

        if ([alert runModal] == NSAlertSecondButtonReturn) {
            // Cancel clicked
            return 0;  
        }
        [alert release]; */

        std::vector<std::string> fileTypes = {"ch8", "CH8", "chip-8", "CHIP-8", "Chip-8", "", "\0"};
        std::vector<std::string> files = openFileDialog("Chip8", "~", fileTypes);

        if (files.empty()) {
            return 0;
        }

        strcpy(rom_name, files[0].c_str());
        
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
