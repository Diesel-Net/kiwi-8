#include "openFileDialog.h"

#ifdef __APPLE__
#include "../MacOS/src/fileDialog.h"
#endif

#ifdef _WIN32
#include "../Windows/src/fileDialog.h"
#endif

void openFileDialog(char *rom_name) {
	/* Call platform specific open file dialog functions */
    #ifdef _WIN32
        openFileDialog(rom_name, "Chip8\0*.ch8\0All\0*.*\0");
    #endif

    #ifdef __APPLE__
        std::vector<std::string> fileTypes = {"ch8", "CH8", "chip-8", "CHIP-8", "Chip-8", "", "\0"};
        std::vector<std::string> files = openFileDialog("Chip8", "~", fileTypes);
        if (!files.empty()) strcpy(rom_name, files[0].c_str());
    #endif
}