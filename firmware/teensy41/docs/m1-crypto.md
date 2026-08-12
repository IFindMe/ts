# M1 — cryptography

M1 establishes the cryptographic foundation required before implementing the Tailscale control/data planes.

## Required primitives

- CSPRNG / hardware-backed entropy source
- X25519 / Curve25519
- ChaCha20-Poly1305
- BLAKE2s
- constant-time operations and secure key wiping

## Test strategy

Every primitive gets known-answer tests on the host and the same vectors on Teensy 4.1. Shared-secret agreement is tested with two independent key pairs. AEAD tests cover valid, modified-ciphertext, modified-AAD, and nonce-reuse rejection at the protocol layer.

Do not put production keys or Tailscale auth keys in examples, tests, CI logs, or source control.

Tailscale nodes use machine and node key pairs; private keys stay on the device while public node keys are distributed by the control plane. Auth keys are only provisioning credentials and are not a replacement for the node's private key.
