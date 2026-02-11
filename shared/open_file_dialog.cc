#include "open_file_dialog.h"

#include <cstdio>
#include <string>
#include <vector>

#ifdef __APPLE__
#include "../macos/src/file_dialog.h"
#endif

#ifdef _WIN32
#include "../windows/src/file_dialog.h"
#endif

#ifdef __linux__
#include "../linux/src/file_dialog.h"
#endif

/* Returns 0 on success, 1 on error (or user hit cancel) */
int openFileDialog(char *rom_name) {

    #if defined(__APPLE__) || defined(__linux__)
    // Common flow for Apple and Linux: both return a vector<string>
    std::vector<std::string> fileTypes = {"ch8", "CH8", "chip-8", "CHIP-8", "Chip-8"};
    const char* defaultDir = ""; // unify behavior: let OS choose last-used/home
    std::vector<std::string> files = openFileDialog("Chip8", defaultDir, fileTypes);
    if (files.empty()) return 1;
    snprintf(rom_name, 256, "%s", files[0].c_str());
    return 0;

    #elif defined(_WIN32)
    // Windows API variant writes directly into buffer and returns int
    return openFileDialog(rom_name, "Chip8\0*.ch8\0All\0*.*\0");

    #else
    return 1;
    #endif
}
