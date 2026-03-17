#ifndef PROFILES_H
#define PROFILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "compat.h"
#include "quirks.h"
#include "sha256.h"
#include <stddef.h>
#include <stdint.h>

typedef struct { uint8_t bytes[32]; } sha256_hash_t;

struct profile {
    sha256_hash_t sha256;
    char rom_name[PATH_MAX];
    struct quirks quirks;
};

/* Initialize the ROM profiles database at startup.
 * Pass a custom file path, or NULL to use default search paths. */
void profiles_init(const char *custom_path);

/* Lookup a ROM profile by SHA256. Returns NULL if not found. */
const struct profile* profile_lookup(const sha256_hash_t *sha256);

/* Save a ROM's quirks to the profile database and INI file.
 * rom_name should be the display name or basename stored in the profile.
 * Also adds the profile to the runtime hashmap immediately. */
void profiles_save(const uint8_t *rom, size_t rom_size, const char *rom_name,
                   const struct quirks *quirks);

#ifdef __cplusplus
}
#endif

#endif /* PROFILES_H */
