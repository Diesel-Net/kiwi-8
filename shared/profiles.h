#ifndef PROFILES_H
#define PROFILES_H

#include <stdint.h>
#include "quirks.h"

struct profile {
    uint32_t crc32;
    char rom_name[256];
    struct quirks quirks;
};

/* Initialize the ROM profiles database at startup.
 * Pass a custom file path, or NULL to use default search paths. */
void profiles_init(const char *custom_path);

/* Lookup a ROM profile by CRC32. Returns NULL if not found. */
const struct profile* profile_lookup(uint32_t crc32);

/* Save current ROM's quirks to the profile database and INI file
 * Requires: chip8.rom, chip8.rom_size, chip8.rom_filename, chip8.quirks
 * Also adds profile to runtime hashmap immediately */
void profiles_save_current(void);

#endif /* PROFILES_H */
