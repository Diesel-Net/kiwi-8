#include "open_file_dialog.h"

#ifdef __APPLE__
#include "../MacOS/src/file_dialog.h"
#endif

#ifdef _WIN32
#include "../Windows/src/file_dialog.h"
#endif

/* Returns 0 on success, 1 on error (or user hit cancel) */
int openFileDialog(char *rom_name) {

    #ifdef __APPLE__
    /* Call MacOS's native open file dialog */
    std::vector<std::string> fileTypes = {"ch8", "CH8", "chip-8", "CHIP-8", "Chip-8", "", "\0"};
    std::vector<std::string> files = openFileDialog("Chip8", "~", fileTypes);
    if (files.empty()) return 1;
    strcpy(rom_name, files[0].c_str());
    return 0;
    #endif

    #ifdef _WIN32
    /* Call Window's native open file dialog */
    return openFileDialog(rom_name, "Chip8\0*.ch8\0All\0*.*\0");
    #endif
}