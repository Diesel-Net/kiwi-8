#include "crc32.h"

/* CRC32 polynomial lookup table (standard) */
static uint32_t crc32_table[256];
static int crc32_table_initialized = 0;

/* Initialize CRC32 lookup table */
static void crc32_init_table(void) {
    uint32_t polynomial = 0xEDB88320;
    int i, j;

    for (i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }

    crc32_table_initialized = 1;
}

uint32_t crc32_compute(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    size_t i;

    /* Initialize table on first call */
    if (!crc32_table_initialized) {
        crc32_init_table();
    }

    /* Process each byte */
    for (i = 0; i < len; i++) {
        uint8_t byte = data[i];
        uint32_t table_idx = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[table_idx];
    }

    return crc ^ 0xFFFFFFFF;
}
