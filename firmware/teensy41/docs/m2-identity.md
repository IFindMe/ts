# M2 — identity and headless provisioning

M2 establishes the local identity material needed for a real Tailscale node.

Tailscale uses separate machine and node key pairs. The machine key identifies the physical device to the control plane; the node key identifies the authenticated node and is used for network access and WireGuard. Private keys stay on the device, while public keys are distributed by the control plane.

References:
- https://tailscale.com/docs/concepts/node-keys
- https://tailscale.com/docs/concepts/tailscale-identity

## Storage

The Teensy stores two independent 32-byte private keys in its EEPROM-backed state record. The record contains:

- magic value
- schema version
- machine private key
- node private key
- CRC32 integrity value

Public keys are derived when needed and are never stored as authoritative state.

## Auth key

M2 accepts a Tailscale auth key beginning with `tskey-auth-`. Tailscale documents auth keys as headless/pre-authentication credentials for devices such as IoT hardware.

Reference: https://tailscale.com/docs/features/access-control/auth-keys

The implementation intentionally does **not** write the auth key to EEPROM. It stays in RAM until cleared. This limits the lifetime of the provisioning credential and avoids turning persistent firmware state into a credential store.

M3 will consume the RAM-only auth key for the actual `/machine/register` flow.

## Serial example

```text
KEYS
AUTH tskey-auth-...
CLEAR
RESET
```

The example only prints public keys. It never prints a private key or the auth-key value.

## Security boundary

M2 does not yet connect to the Tailscale control plane. It provides the persistent cryptographic identity and safe credential handoff required by M3.
