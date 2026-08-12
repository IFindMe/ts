#include "ts_crypto.h"
#include "blake2s.h"

#include <Arduino.h>
#include <Entropy.h>
#include <monocypher.h>

namespace ts::crypto {

bool randomBytes(uint8_t *out, size_t length) {
  if (out == nullptr && length != 0) return false;
  for (size_t i = 0; i < length; ) {
    const uint32_t value = Entropy.random();
    const size_t take = (length - i < sizeof(value)) ? length - i : sizeof(value);
    for (size_t j = 0; j < take; ++j) {
      out[i + j] = static_cast<uint8_t>(value >> (8 * j));
    }
    i += take;
  }
  return true;
}

bool x25519PublicKey(uint8_t publicKey[kKeySize],
                    const uint8_t privateKey[kKeySize]) {
  if (publicKey == nullptr || privateKey == nullptr) return false;
  crypto_x25519_public_key(publicKey, privateKey);
  return true;
}

bool x25519(uint8_t sharedSecret[kKeySize],
            const uint8_t privateKey[kKeySize],
            const uint8_t peerPublicKey[kKeySize]) {
  if (sharedSecret == nullptr || privateKey == nullptr || peerPublicKey == nullptr) {
    return false;
  }
  crypto_x25519(sharedSecret, privateKey, peerPublicKey);
  uint8_t nonzero = 0;
  for (size_t i = 0; i < kKeySize; ++i) nonzero |= sharedSecret[i];
  return nonzero != 0;
}

bool blake2s(uint8_t digest[kHashSize], const uint8_t *message, size_t length) {
  return ts_blake2s(digest, kHashSize, nullptr, 0, message, length) == 0;
}

bool aeadIetfSeal(uint8_t *ciphertext,
                  uint8_t tag[kAeadTagSize],
                  const uint8_t key[kKeySize],
                  const uint8_t nonce[kIetfNonceSize],
                  const uint8_t *aad,
                  size_t aadLength,
                  const uint8_t *plaintext,
                  size_t plaintextLength) {
  if (ciphertext == nullptr || tag == nullptr || key == nullptr || nonce == nullptr ||
      (aadLength != 0 && aad == nullptr) ||
      (plaintextLength != 0 && plaintext == nullptr)) {
    return false;
  }

  crypto_aead_ctx ctx;
  crypto_aead_init_ietf(&ctx, key, nonce);
  crypto_aead_write(&ctx, ciphertext, tag, aad, aadLength, plaintext, plaintextLength);
  crypto_wipe(&ctx, sizeof(ctx));
  return true;
}

bool aeadIetfOpen(uint8_t *plaintext,
                  const uint8_t tag[kAeadTagSize],
                  const uint8_t key[kKeySize],
                  const uint8_t nonce[kIetfNonceSize],
                  const uint8_t *aad,
                  size_t aadLength,
                  const uint8_t *ciphertext,
                  size_t ciphertextLength) {
  if (plaintext == nullptr || tag == nullptr || key == nullptr || nonce == nullptr ||
      (aadLength != 0 && aad == nullptr) ||
      (ciphertextLength != 0 && ciphertext == nullptr)) {
    return false;
  }

  crypto_aead_ctx ctx;
  crypto_aead_init_ietf(&ctx, key, nonce);
  const bool ok = crypto_aead_read(&ctx, plaintext, tag, aad, aadLength,
                                   ciphertext, ciphertextLength) == 0;
  crypto_wipe(&ctx, sizeof(ctx));
  return ok;
}

}  // namespace ts::crypto
