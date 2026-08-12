# Teensy 4.1 Tailscale M2 Identity Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Establish persistent Tailscale-style machine and node identity on Teensy 4.1 and a safe headless auth-key provisioning path without implementing control-plane registration yet.

**Architecture:** Generate two independent X25519 private keys: one persistent machine key and one persistent node key. Store only those private keys in the Teensy EEPROM-backed state record; derive public keys as needed. Auth keys are provisioning credentials held only in RAM and explicitly cleared after use. M2 stops before the `/machine/register` network protocol.

**Tech Stack:** Teensy 4.1, Arduino EEPROM, existing M1 crypto API, C++17-compatible Arduino toolchain, host-side C++ tests.

## Global Constraints

- Never commit an auth key, machine private key, or node private key.
- Machine and node private keys are independent 32-byte X25519 secrets.
- Public keys are derived, not persisted separately.
- Persistent state has a magic value, schema version, and CRC32 integrity check.
- Auth keys must start with `tskey-auth-`, be case-sensitive, and remain in RAM only.
- M2 does not implement `/machine/register`, Noise control transport, or network-map parsing.
- Private key material is wiped from temporary buffers after use.

---

### Task 1: Add failing host tests for auth-key validation and state integrity

**Files:**
- Create: `firmware/teensy41/test/m2_identity_tests.cpp`
- Create: `firmware/teensy41/include/ts_auth.h`

**Interfaces:**
- `bool ts::auth::isValidAuthKey(const char*, size_t)`.
- `uint32_t ts::state::crc32(const uint8_t*, size_t)`.

- [ ] **Step 1: Write failing tests** for valid `tskey-auth-*`, wrong prefix, empty key, overlong key, and CRC32 known vector for `123456789`.
- [ ] **Step 2: Run the tests** and verify they fail because the functions do not exist yet.
- [ ] **Step 3: Commit the red tests.**

---

### Task 2: Implement auth-key validation and integrity primitives

**Files:**
- Create: `firmware/teensy41/src/ts_auth.cpp`
- Create: `firmware/teensy41/include/ts_state.h`
- Create: `firmware/teensy41/src/ts_state.cpp`

**Interfaces:**
- `ts::auth::isValidAuthKey(...)` validates without storing the key.
- `ts::state::crc32(...)` returns standard CRC-32/ISO-HDLC.

- [ ] **Step 1:** Implement the minimal validation and CRC32.
- [ ] **Step 2:** Run host tests and verify green.
- [ ] **Step 3:** Ensure validation never logs the secret value.
- [ ] **Step 4:** Commit.

---

### Task 3: Implement persistent machine/node key storage

**Files:**
- Create: `firmware/teensy41/include/ts_identity.h`
- Create: `firmware/teensy41/src/ts_identity.cpp`

**Interfaces:**
- `class TsIdentity { bool begin(); bool initialized() const; const uint8_t* machinePrivate() const; const uint8_t* nodePrivate() const; bool machinePublic(uint8_t[32]) const; bool nodePublic(uint8_t[32]) const; bool reset(); bool setAuthKey(const char*, size_t); bool hasAuthKey() const; const char* authKey() const; void clearAuthKey(); }`

- [ ] **Step 1:** Define a fixed EEPROM state record with magic, version, flags, 32-byte machine private key, 32-byte node private key, and CRC32.
- [ ] **Step 2:** On boot, validate magic/version/CRC; if invalid, generate two independent keys with the M1 RNG and persist them once.
- [ ] **Step 3:** Derive public keys through M1 X25519 functions.
- [ ] **Step 4:** Implement reset by wiping RAM copies and replacing EEPROM state with a fresh key pair.
- [ ] **Step 5:** Keep auth keys in a fixed-size RAM buffer only; validate before copying and wipe on `clearAuthKey()`.
- [ ] **Step 6:** Run the Teensy build through CI.
- [ ] **Step 7:** Commit.

---

### Task 4: Add M2 physical self-test and provisioning example

**Files:**
- Create: `firmware/teensy41/examples/m2_identity/m2_identity.ino`
- Create: `firmware/teensy41/docs/m2-identity.md`

**Interfaces:**
- The example prints only public key material.
- Serial command `AUTH <key>` provisions a RAM-only auth key.
- Serial command `CLEAR` wipes the auth key buffer.

- [ ] **Step 1:** On boot, report whether state was loaded or freshly generated without printing private keys.
- [ ] **Step 2:** Print machine/node public keys in hex.
- [ ] **Step 3:** Accept a single `AUTH tskey-auth-...` command and report only `AUTH KEY ACCEPTED` or `AUTH KEY REJECTED`.
- [ ] **Step 4:** `CLEAR` wipes the RAM auth buffer.
- [ ] **Step 5:** Document that M3 will consume the RAM auth key for `/machine/register` and that M2 does not yet contact the control server.
- [ ] **Step 6:** Commit.

---

## M2 Acceptance Criteria

1. Two independent persistent private keys exist after first boot.
2. Reboot preserves both key pairs.
3. Corrupting the state record causes safe regeneration instead of loading corrupted keys.
4. Public keys can be derived deterministically from the stored private keys.
5. Auth keys are accepted only with the current `tskey-auth-` prefix and never written to EEPROM.
6. Auth key memory can be explicitly wiped.
7. Serial logs never reveal private keys or auth-key contents.
8. No `/machine/register` networking is claimed or implemented yet.
