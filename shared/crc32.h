#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include <stddef.h>

/* Compute CRC32 checksum of data buffer
 * Returns 32-bit CRC32 value
 * Compatible with standard CRC32 (used in zlib, PNG, etc.) */
uint32_t crc32_compute(const uint8_t* data, size_t len);

#endif /* CRC32_H */
