#include "open_file_dialog.h"
#include "compat.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int open_file_dialog(char *rom_filepath, char *filters) {
    /* open file dialogue */
    char cwd[PATH_MAX];
    GetCurrentDirectory(PATH_MAX, cwd);

    OPENFILENAME ofn;

    char szFile[PATH_MAX];

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

    /* change current working directory back to location of executable */
    SetCurrentDirectory(cwd);

    if (!GetOpenFileName( &ofn)) {
        /* user hit cancel */
        return 1;
    }

    strcpy(rom_filepath, szFile);
    return 0;
}

int open_file_dialog(char *rom_filepath) {
    return open_file_dialog(rom_filepath, "Chip8\0*.ch8\0All\0*.*\0");
}
