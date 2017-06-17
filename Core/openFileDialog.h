#ifndef OPEN_FILE_DIALOG_H
#define OPEN_FILE_DIALOG_H

#ifdef __APPLE__
#include <limits.h> /* PATH_MAX */
#endif

#ifdef _WIN32
#include <windows.h> /* MAX_PATH */
#define PATH_MAX MAX_PATH /* PATH_MAX */
#endif

void openFileDialog(char *rom_name);

#endif
