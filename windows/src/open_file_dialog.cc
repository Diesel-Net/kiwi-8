#include "open_file_dialog.h"
#include "compat.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int open_file_dialog(char *rom_filepath, size_t size, char *filters) {
    /* open file dialogue */
    char cwd[size];
    GetCurrentDirectory(size, cwd);

    OPENFILENAME ofn;

    char szFile[size];

    /* open a file name */
    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL  ;
    ofn.lpstrFile = szFile ;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof( szFile );
    ofn.lpstrFilter = filters;
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

    strncpy(rom_filepath, szFile, size);
    return 0;
}

int open_file_dialog(char *rom_filepath, size_t size) {
    return open_file_dialog(rom_filepath, size, "Chip8\0*.ch8\0All\0*.*\0");
}
