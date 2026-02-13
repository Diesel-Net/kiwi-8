#include "sha256.h"
#include <string.h>
#include <stdio.h>
#include <openssl/evp.h>

sha256_hash_t sha256_compute(const uint8_t* data, size_t len) {
    sha256_hash_t result;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();

    if (!mdctx) {
        memset(&result, 0, sizeof(result));
        return result;
    }

    if (!EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        EVP_MD_CTX_free(mdctx);
        memset(&result, 0, sizeof(result));
        return result;
    }

    if (!EVP_DigestUpdate(mdctx, data, len)) {
        EVP_MD_CTX_free(mdctx);
        memset(&result, 0, sizeof(result));
        return result;
    }

    unsigned int digest_len = 0;
    if (!EVP_DigestFinal_ex(mdctx, result.bytes, &digest_len)) {
        EVP_MD_CTX_free(mdctx);
        memset(&result, 0, sizeof(result));
        return result;
    }

    EVP_MD_CTX_free(mdctx);
    return result;
}

void sha256_to_hex(const sha256_hash_t *hash, char *dst, size_t dst_size) {
    if (dst_size < 65) return;  /* 64 hex chars + null terminator */

    for (int i = 0; i < 32; i++) {
        snprintf(dst + i*2, 3, "%02x", hash->bytes[i]);
    }
    dst[64] = '\0';
}

int sha256_equal(const sha256_hash_t *a, const sha256_hash_t *b) {
    for (int i = 0; i < 32; i++) {
        if (a->bytes[i] != b->bytes[i]) {
            return 0;
        }
    }
    return 1;
}
