/*
Author: Thomas Daley
Date: September 8, 2016
*/

#include "../../Core/Chip8.h"
#include "resource.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){

    char cwd[MAX_PATH];
    
    char *rom_name;

    /* Defaults */
    bool fullscreen = 0;
    bool load_store_quirk = 0;
    bool shift_quirk = 0;
    bool vwrap = 1;
    unsigned char R = 200;
    unsigned char G = 170;
    unsigned char B = 255;

    if (argc < 2) {

        /*
        int result = MessageBox (NULL , "Click OK to select a ROM file.\n\n"
                            "Alternatively, you may launch Chip8 with the command line.\n\n"
                            "Usage: Chip8 PATH_TO_ROM [-FLS] [R G B]\n"
                            "       -F\t\tLaunch in fullscreen\n"
                            "       -L\t\tEnable load/store quirk\n"
                            "       -S\t\tEnable shift quirk\n"
                            "       -V\t\tDisable vertical wrapping\n"
                            "       R G B\t\tForeground color in RGB format,\n"
                            "            \t\t3 numbers from 0-255\n\n\n\n"
                            "Enjoy!\n\nThomas Daley", VERSION, MB_OKCANCEL);
        if (result == IDCANCEL) {
            return 0;
        } */

        /* Open file dialogue */
        GetCurrentDirectory(MAX_PATH, cwd);

        OPENFILENAME ofn;

        char szFile[MAX_PATH];

        /* open a file name */
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
            return 0;
        }

        /* Change current working directory back to location of executable */
        SetCurrentDirectory(cwd);

        rom_name = szFile;
        
    } else {
        rom_name = argv[1];
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
