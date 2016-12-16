/*
Author: Thomas Daley
Date: September 8, 2016
main.cc

Purpose: Atari Chip-8 emulator
*/

#include "chip8.h"

#include <Commdlg.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

int main(int argc, char **argv){

    char cwd[MAX_PATH];
    
    char *rom;
    int fullscreen = 0;
    int R = 0;
    int G = 255;
    int B = 200;

    if (argc < 2) {

        GetCurrentDirectory(MAX_PATH, cwd);

        int result = MessageBox (NULL , "Click OK to select a ROM file.\n\n"
                            "Alternatively you may launch Chip8 with the command line instead.\n\n"
                            "Usage: Chip8 PATH_TO_ROM [-F] [R G B]\n"
                            "       -F\t\tLaunch in Fullscreen\n"
                            "       R G B\t\tRender color in RGB format, 3 numbers between 0-255\n\n\n\n"
                            "Enjoy!\n\n-Thomas Daley", "Chip8 v1.0" , MB_OKCANCEL);
        if (result == IDCANCEL) {
            return EXIT_SUCCESS;
        }

        OPENFILENAME ofn;

        char szFile[MAX_PATH];

        // open a file name
        ZeroMemory( &ofn , sizeof( ofn));
        ofn.lStructSize = sizeof ( ofn );
        ofn.hwndOwner = NULL  ;
        ofn.lpstrFile = szFile ;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof( szFile );
        ofn.lpstrFilter = "Chip8\0*.ch8\0All\0*.*\0";
        ofn.nFilterIndex =1;
        ofn.lpstrFileTitle = NULL ;
        ofn.nMaxFileTitle = 0 ;
        ofn.lpstrInitialDir=NULL ;
        ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ;

        if (!GetOpenFileName( &ofn)) {
            return EXIT_SUCCESS;
        }

        /* Change current working directory back to location of executable */
        SetCurrentDirectory(cwd);

        rom = szFile;
        
    } else {
        rom = argv[1];
    }

    if (argc >= 3 && (strcmp(argv[2], "-F") == 0)) {
        fullscreen = 1;
    }

    if (argc > 2 + fullscreen) {
        R = atoi(argv[2 + fullscreen]);

        printf("R: %d\n", R);
    }

    if (argc > 3 + fullscreen) {
        G = atoi(argv[3 + fullscreen]);

        printf("G: %d\n", G);
    }
    

    if (argc > 4 + fullscreen) {
        B = atoi(argv[4 + fullscreen]);

        printf("B: %d\n", B);
    }
    
    Chip8 chip = Chip8();

    if (chip.Initialize(fullscreen, R, G, B)) {
        return EXIT_FAILURE;
    }

    printf("Loading %s\n", rom);

    if (chip.Load(rom)) {
        return EXIT_FAILURE;
    }
    
    chip.Run();

    printf("Clean Exit.\n");
    return EXIT_SUCCESS;
}
