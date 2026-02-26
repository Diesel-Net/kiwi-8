#ifndef OPEN_FILE_DIALOG_H
#define OPEN_FILE_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Opens a file dialog to select a ROM file.
 * The selected file path is copied to `rom_filepath`.
 * The `size` parameter specifies the size of the `rom_filepath` buffer.
 * Returns 1 if a file was selected, 0 if cancelled, or -1 on error.
 */
int open_file_dialog(char *rom_filepath, size_t size);

#ifdef __cplusplus
}
#endif

#endif // OPEN_FILE_DIALOG_H
