/*
Author: Thomas Daley
Date: September 8, 2016
main.cc

Purpose: Atari Chip-8 emulator

on MacOS: 
    open ./Chip8.app --args "/ABSOLUTE_PATH_TO_ROM.ch8" -F 255 255 255
*/

#include "../../Core/Chip8.h"
#include "fileDialog.mm"

#include <string.h>


int main(int argc, char **argv){

    char rom[PATH_MAX];
    memset(rom, 0 , PATH_MAX);

    int fullscreen = 0;
    int R = 0;
    int G = 255;
    int B = 200;

    if (argc < 2 || argv[1][0] == '-') {

        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@"Chip8 v1.0"];
        [alert setInformativeText:@"Click OK to select a ROM file.\n\n"
                                "Alternatively, you may launch Chip8 with the command line.\n\n"
                                "Usage: Chip8 PATH_TO_ROM [-F] [R G B]\n"
                                "  -F\t\tLaunch in Fullscreen\n"
                                "  R G B\tRender color in RGB format, 3 numbers \n\t\tbetween 0-255\n\n\n\n"
                                "Enjoy!\n\n-Thomas Daley"];
        [alert setAlertStyle:NSWarningAlertStyle];

        if ([alert runModal] == NSAlertSecondButtonReturn) {
            // Cancel clicked
            return 0;  
        }
        [alert release];

        std::vector<std::string> fileTypes = {"ch8\0", "CH8\0", "chip-8\0", "CHIP-8\0", "Chip-8\0"};
        std::vector<std::string> files = openFileDialog("Chip8\0", "~\0", fileTypes);

        
        if (files.empty()) {
            return 0;
        }

        strcpy(rom, files[0].c_str());
        
    } else {
        strcpy(rom, argv[1]);
    }

    if (argc >= 3 && (strcmp(argv[2], "-F") == 0)) {
        fullscreen = 1;
    }

    if (argc > 2 + fullscreen) {
        R = atoi(argv[2 + fullscreen]);

        fprintf(stderr, "R: %d\n", R);
    }

    if (argc > 3 + fullscreen) {
        G = atoi(argv[3 + fullscreen]);

        fprintf(stderr, "G: %d\n", G);
    }
    

    if (argc > 4 + fullscreen) {
        B = atoi(argv[4 + fullscreen]);

        fprintf(stderr, "B: %d\n", B);
    }
   
    Chip8 chip = Chip8();

    if (chip.Initialize(fullscreen, R, G, B)) {
        return 1;
    }

    fprintf(stderr, "Loading %s\n", rom);

    if (chip.Load(rom)) {
        return 1;
    }
    
    chip.Run();
    fprintf(stderr, "Clean Exit.\n");
    
    return 0;
}
