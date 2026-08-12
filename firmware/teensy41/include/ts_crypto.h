#pragma once

#include <stddef.h>
#include <stdint.h>

namespace ts::crypto {

constexpr size_t kKeySize = 32;
constexpr size_t kHashSize = 32;
constexpr size_t kAeadTagSize = 16;
constexpr size_t kIetfNonceSize = 12;

bool randomBytes(uint8_t *out, size_t length);

bool x25519PublicKey(uint8_t publicKey[kKeySize],
                    const uint8_t privateKey[kKeySize]);

bool x25519(uint8_t sharedSecret[kKeySize],
            const uint8_t privateKey[kKeySize],
            const uint8_t peerPublicKey[kKeySize]);

bool blake2s(uint8_t digest[kHashSize],
            const uint8_t *message,
            size_t length);

bool aeadIetfSeal(uint8_t *ciphertext,
                  uint8_t tag[kAeadTagSize],
                  const uint8_t key[kKeySize],
                  const uint8_t nonce[kIetfNonceSize],
                  const uint8_t *aad,
                  size_t aadLength,
                  const uint8_t *plaintext,
                  size_t plaintextLength);

bool aeadIetfOpen(uint8_t *plaintext,
                  const uint8_t tag[kAeadTagSize],
                  const uint8_t key[kKeySize],
                  const uint8_t nonce[kIetfNonceSize],
                  const uint8_t *aad,
                  size_t aadLength,
                  const uint8_t *ciphertext,
                  size_t ciphertextLength);

}  // namespace ts::crypto
