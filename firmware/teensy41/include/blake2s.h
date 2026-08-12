#pragma once

#include <stddef.h>
#include <stdint.h>

int ts_blake2s(uint8_t *out,
               size_t outlen,
               const uint8_t *key,
               size_t keylen,
               const uint8_t *in,
               size_t inlen);
