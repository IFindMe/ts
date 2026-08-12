#pragma once

#include <stddef.h>
#include <stdint.h>

namespace ts::crypto {

constexpr size_t kKeySize = 32;
constexpr size_t kHashSize = 32;

// M1 interface. The implementation will be backed by an audited crypto
// library after host and Teensy test vectors are established.
bool randomBytes(uint8_t *out, size_t length);
bool x25519PublicKey(uint8_t publicKey[kKeySize], const uint8_t privateKey[kKeySize]);
bool x25519(uint8_t sharedSecret[kKeySize], const uint8_t privateKey[kKeySize], const uint8_t peerPublicKey[kKeySize]);
bool blake2s(uint8_t digest[kHashSize], const uint8_t *message, size_t length);

}  // namespace ts::crypto
