#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

/* SHA256 hash value (256 bits = 32 bytes) */
typedef struct {
    uint8_t bytes[32];
} sha256_hash_t;

/* Compute SHA256 hash of data buffer using OpenSSL
 * Returns 32-byte SHA256 hash */
sha256_hash_t sha256_compute(const uint8_t* data, size_t len);

/* Convert hash to hex string (65 bytes including null terminator)
 * dst must be at least 65 bytes */
void sha256_to_hex(const sha256_hash_t *hash, char *dst, size_t dst_size);

/* Compare two SHA256 hashes */
int sha256_equal(const sha256_hash_t *a, const sha256_hash_t *b);

#endif /* SHA256_H */
