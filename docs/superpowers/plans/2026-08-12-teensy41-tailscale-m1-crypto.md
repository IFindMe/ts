# Teensy 4.1 Tailscale M1 Crypto Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Establish the cryptographic primitives required by Tailscale/WireGuard on Teensy 4.1 and verify them with standard test vectors.

**Architecture:** Use Monocypher 4.0.3 for X25519 and RFC 8439 ChaCha20-Poly1305 because it is small, portable, and embedded-oriented. Implement BLAKE2s separately from RFC 7693 because WireGuard requires BLAKE2s while Monocypher exposes BLAKE2b. Use the Teensy hardware entropy facility exposed by the Entropy library for random bytes.

**Tech Stack:** C/C++, PlatformIO, Teensy 4.1, Monocypher 4.0.3, RFC 7693 BLAKE2s, Teensy Entropy.

## Global Constraints

- No Tailscale auth key or private production key may be committed.
- X25519 outputs must be checked for the all-zero shared secret before use.
- WireGuard uses BLAKE2s, not BLAKE2b.
- ChaCha20-Poly1305 must use the IETF 96-bit nonce form.
- Secret buffers are wiped after use where practical.
- Test vectors must be deterministic and independent of device-generated randomness.

### Task 1: Crypto dependency and interface

**Files:**
- Modify: `firmware/teensy41/platformio.ini`
- Modify: `firmware/teensy41/include/ts_crypto.h`
- Create: `firmware/teensy41/src/ts_crypto.cpp`

Add Monocypher 4.0.3 as an exact Git dependency. Expose `randomBytes`, X25519 key/public/shared-secret operations, BLAKE2s, and IETF ChaCha20-Poly1305 seal/open operations.

### Task 2: BLAKE2s implementation

**Files:**
- Create: `firmware/teensy41/include/blake2s.h`
- Create: `firmware/teensy41/src/blake2s.cpp`
- Create: `firmware/teensy41/test/blake2s_host.c`

Implement BLAKE2s-256 from RFC 7693 with keyed and unkeyed operation. Validate empty input, `abc`, and a keyed test against independent reference output before using it in the embedded wrapper.

### Task 3: X25519 and AEAD known-answer tests

**Files:**
- Create: `firmware/teensy41/test/m1_vectors.cpp`
- Create: `firmware/teensy41/test/M1-CRYPTO.md`

Use RFC 7748 X25519 vectors and RFC 8439 ChaCha20-Poly1305 vectors. Verify X25519 public/shared results, successful AEAD open, and failure after ciphertext/AAD tampering.

### Task 4: Embedded self-test

**Files:**
- Create: `firmware/teensy41/examples/m1_crypto/m1_crypto.ino`

Run the deterministic vectors on the physical Teensy and print PASS/FAIL without printing secrets beyond public test-vector values.

### Acceptance

M1 is complete when the host tests pass, the PlatformIO Teensy build passes, and the physical Teensy self-test reports PASS for BLAKE2s, X25519, AEAD encryption/decryption, tamper rejection, entropy generation, and secure wiping calls compile successfully.
