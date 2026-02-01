#ifndef PROFILES_H
#define PROFILES_H

#include <stdint.h>
#include "quirks.h"

struct profile {
    struct quirks quirks;
};

/* Initialize the ROM profiles database at startup */
void profiles_init(void);

/* Lookup a ROM profile by CRC32. Returns NULL if not found. */
const struct profile* profile_lookup(uint32_t crc32);

#endif /* PROFILES_H */
