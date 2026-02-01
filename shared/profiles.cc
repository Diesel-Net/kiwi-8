#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include "profiles.h"
#include <stddef.h>

/* ROM profile hashmap: CRC32 -> profile */
static struct { uint32_t key; struct profile value; } *profile_map = NULL;

/* Populate the database with known ROM profiles */
static void profiles_populate(void) {
    struct profile p;
    uint32_t crc;

    /* Example entry: placeholder CRC32 0xDEADBEEF */
    /* Set all quirks to 0 (false) as example */
    p.quirks.load_store_quirk = 0;
    p.quirks.shift_quirk = 0;
    p.quirks.jump_quirk = 0;
    p.quirks.logic_vf_quirk = 0;
    p.quirks.i_overflow_quirk = 0;
    p.quirks.draw_flag_quirk = 0;
    p.quirks.vwrap = 0;
    p.quirks.hwrap = 0;

    crc = 0xDEADBEEF;
    hmput(profile_map, crc, p);

    /* Add more ROM profiles here as needed */
}

void profiles_init(void) {
    if (profile_map == NULL) {
        profiles_populate();
    }
}

const struct profile* profile_lookup(uint32_t crc32) {
    ptrdiff_t idx;
    idx = hmgeti(profile_map, crc32);
    if (idx < 0) {
        return NULL;
    }
    return &profile_map[idx].value;
}
