#ifndef PROFILES_H
#define PROFILES_H

#include <stdint.h>
#include "quirks.h"
#include "sha256.h"

typedef struct { uint8_t bytes[32]; } sha256_hash_t;

struct profile {
    sha256_hash_t sha256;
    char rom_name[256];
    struct quirks quirks;
};

/* Initialize the ROM profiles database at startup.
 * Pass a custom file path, or NULL to use default search paths. */
void profiles_init(const char *custom_path);

/* Lookup a ROM profile by SHA256. Returns NULL if not found. */
const struct profile* profile_lookup(const sha256_hash_t *sha256);

/* Save current ROM's quirks to the profile database and INI file
 * Requires: chip8.rom, chip8.rom_size, chip8.rom_filename, chip8.quirks
 * Also adds profile to runtime hashmap immediately */
void profiles_save_current(void);

#endif /* PROFILES_H */
