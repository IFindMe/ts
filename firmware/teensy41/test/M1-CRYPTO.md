# M1 crypto verification

M1 validates the primitives required by the later Tailscale/WireGuard implementation.

## Implemented

- X25519 via Monocypher 4.0.3
- ChaCha20-Poly1305 with the IETF 96-bit nonce form via Monocypher 4.0.3
- BLAKE2s-256 implemented locally from RFC 7693
- Teensy 4.1 hardware entropy through the Entropy library
- all-zero X25519 shared-secret rejection
- secret context wiping after AEAD operations

## Deterministic tests

`m1_vectors.cpp` contains deterministic known-answer checks for:

- RFC 7748 X25519 public keys and shared secret
- BLAKE2s-256 of `abc`
- ChaCha20-Poly1305 IETF encryption/decryption
- ciphertext tamper rejection
- entropy API availability

`blake2s_host.cpp` runs the BLAKE2s implementation independently on the host and checks empty, `abc`, and keyed `abc` outputs.

## Physical Teensy test

`examples/m1_crypto/m1_crypto.ino` exercises X25519, BLAKE2s, ChaCha20-Poly1305, tamper rejection, and hardware entropy on the actual Teensy 4.1.

Run at 115200 baud and require every primitive to report `[PASS]` before continuing to M2.

No Tailscale auth key, node private key, machine private key, or other production credential belongs in this directory or CI logs.
