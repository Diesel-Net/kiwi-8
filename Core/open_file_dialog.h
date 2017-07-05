#ifndef OPEN_FILE_DIALOG_H
#define OPEN_FILE_DIALOG_H

#if defined( __APPLE__ ) || defined ( __Linux__ )
#include <limits.h> /* PATH_MAX */
#endif

#ifdef _WIN32
#include <windows.h> /* MAX_PATH */
#define PATH_MAX MAX_PATH /* PATH_MAX */
#endif

int openFileDialog(char *rom_name);

#endif
