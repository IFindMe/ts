#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "ts_crypto.h"

static bool equalBytes(const uint8_t *a, const uint8_t *b, size_t n) {
  return memcmp(a, b, n) == 0;
}

static void printHex(const char *label, const uint8_t *data, size_t len) {
  Serial.print(label);
  for (size_t i = 0; i < len; ++i) {
    if (data[i] < 16) Serial.print('0');
    Serial.print(data[i], HEX);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== Tailscale Teensy 4.1 / M1 ===");

  static const uint8_t privateKey[32] = {
      0x77,0x07,0x6d,0x0a,0x73,0x18,0xa5,0x7d,0x3c,0x16,0xc1,0x72,0x51,0xb2,0x66,0x45,
      0xdf,0x4c,0x2f,0x87,0xeb,0xc0,0x99,0x2a,0xb1,0x77,0xfb,0xa5,0x1d,0xb9,0x2c,0x2a};
  static const uint8_t expectedPublic[32] = {
      0x85,0x20,0xf0,0x09,0x89,0x30,0xa7,0x54,0x74,0x8b,0x7d,0xdc,0xb4,0x3e,0xf7,0x5a,
      0x0d,0xbf,0x3a,0x0d,0x26,0x38,0x1a,0xf4,0xeb,0xa4,0xa9,0x8e,0xaa,0x9b,0x4e,0x6a};

  uint8_t publicKey[32];
  if (ts::crypto::x25519PublicKey(publicKey, privateKey) && equalBytes(publicKey, expectedPublic, 32)) {
    Serial.println("[PASS] X25519 RFC 7748 public-key vector");
    printHex("public=", publicKey, 32);
  } else {
    Serial.println("[FAIL] X25519 public-key vector");
  }

  const uint8_t abc[] = {'a', 'b', 'c'};
  const uint8_t expectedBlake[32] = {
      0x50,0x8c,0x5e,0x8c,0x32,0x7c,0x14,0xe2,0xe1,0xa7,0x2b,0xa3,0x4e,0xeb,0x45,0x2f,
      0x37,0x45,0x8b,0x20,0x9e,0xd6,0x3a,0x29,0x4d,0x99,0x9b,0x4c,0x86,0x67,0x59,0x82};
  uint8_t digest[32];
  if (ts::crypto::blake2s(digest, abc, sizeof(abc)) && equalBytes(digest, expectedBlake, 32)) {
    Serial.println("[PASS] BLAKE2s-256 abc vector");
  } else {
    Serial.println("[FAIL] BLAKE2s-256 abc vector");
  }

  uint8_t entropy[32];
  if (ts::crypto::randomBytes(entropy, sizeof(entropy))) {
    uint8_t nonzero = 0;
    for (uint8_t b : entropy) nonzero |= b;
    Serial.println(nonzero ? "[PASS] Hardware entropy" : "[FAIL] Hardware entropy returned all zero");
  } else {
    Serial.println("[FAIL] Hardware entropy API");
  }

  Serial.println("M1 self-test complete.");
}

void loop() {}
