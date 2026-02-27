#include "open_file_dialog.h"
#include "compat.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

int open_file_dialog(char *rom_filepath, size_t size, const char *filters) {
    if (rom_filepath == NULL || size == 0 || size > MAXDWORD) {
        return -1;
    }

    const DWORD buffer_size = (DWORD)size;

    /* open file dialogue */
    std::vector<char> cwd(MAX_PATH, '\0');
    if (GetCurrentDirectoryA((DWORD)cwd.size(), cwd.data()) == 0) {
        return -1;
    }

    OPENFILENAME ofn;

    std::vector<char> szFile(buffer_size, '\0');

    /* open a file name */
    ZeroMemory( &ofn , sizeof( ofn));
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner = NULL  ;
    ofn.lpstrFile = szFile.data() ;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = buffer_size;
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

    snprintf(rom_filepath, size, "%s", szFile.data());
    return 0;
}

int open_file_dialog(char *rom_filepath, size_t size) {
    return open_file_dialog(rom_filepath, size, "Chip8\0*.ch8\0All\0*.*\0");
}
