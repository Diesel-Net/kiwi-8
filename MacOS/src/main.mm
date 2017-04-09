/*
Author: Thomas Daley
Date: September 8, 2016

on MacOS: 
    open ./Chip8.app --args "/ABSOLUTE_PATH_TO_ROM.ch8" -F 255 255 255
*/

#include "../../Core/Chip8.h"
#include "fileDialog.mm"

#include <string.h>


int main(int argc, char **argv){

    char rom[PATH_MAX];
    memset(rom, 0 , PATH_MAX);

    /* Defaults */
    unsigned int fullscreen = 0;
    unsigned int load_store_quirk = 0;
    unsigned int shift_quirk = 0;
    unsigned char R = 0;
    unsigned char G = 255;
    unsigned char B = 200;

    if (argc < 2 || argv[1][0] == '-') {

        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@TITLE];
        [alert setInformativeText:@"Click OK to select a ROM file.\n\n"
                                "Alternatively, you may launch Chip8 with the command line.\n\n"
                                "Usage: Chip8 PATH_TO_ROM [-FLS] [R G B]\n"
                                "       -F\t\tLaunch in fullscreen\n"
                                "       -L\t\tEnable load/store quirk\n"
                                "       -S\t\tEnable shift quirk\n"
                                "       R G B\t\tForeground color in RGB format,\n"
                                "            \t\t3 numbers from 0-255\n\n\n\n"
                                "Enjoy!\n\nThomas Daley"];

        if ([alert runModal] == NSAlertSecondButtonReturn) {
            /* Cancel clicked */
            return 0;  
        }
        [alert release];

        std::vector<std::string> fileTypes = {"ch8\0", "CH8\0", "chip-8\0", "CHIP-8\0", "Chip-8\0", "\0"};
        std::vector<std::string> files = openFileDialog("Chip8\0", "~\0", fileTypes);

        
        if (files.empty()) {
            return 0;
        }

        strcpy(rom, files[0].c_str());
        
    } else {
        strcpy(rom, argv[1]);
    }

    if (argc >= 3) {

        char *pos = argv[2];
        if (*pos == '-') {
            
            pos++;
            int len = strlen(pos);

            for (int i = 0; i < len; i++) {
                /* Parse the options */
                if (*pos == 'F') {
                    fullscreen = 1;
                }
                if (*pos == 'L') {
                    load_store_quirk = 1;
                }
                if (*pos == 'S') {
                    shift_quirk = 1;
                }
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
    chip.Initialize(fullscreen, load_store_quirk, shift_quirk, R, G, B);

    if (chip.Load(rom)) {
        return 1;
    }
    
    chip.Run();
    fprintf(stderr, "Main thread terminated.\n");
    return 0;
}
