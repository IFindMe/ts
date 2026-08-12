# Teensy 4.1 Tailscale Client — Initial Design

## Goal

Build an embedded Tailscale-compatible client for a Teensy 4.1 using its native Ethernet hardware. The first authentication mechanism will be a pre-generated Tailscale auth key; no browser/OAuth flow will be implemented on-device.

## Target hardware

- Teensy 4.1
- Native Ethernet PHY/wiring already physically installed
- Native Ethernet networking stack first
- No W5500 requirement in v0.1

## Architecture

1. Ethernet/IP transport
2. TLS/HTTPS client for the Tailscale control plane
3. Tailscale device identity and key storage
4. Network-map handling
5. WireGuard cryptographic/data plane
6. UDP endpoint management
7. DISCO/NAT traversal
8. DERP fallback
9. Embedded Tailscale virtual-interface/routing integration

## Authentication

Use a Tailscale auth key for initial node registration. The auth key is provisioning material and must not be committed to source control. The Teensy should generate and persist its own machine/node cryptographic keys where practical.

## Milestones

### M0 — Hardware/network proof
- Initialize native Ethernet
- DHCP/static IPv4
- DNS
- TCP connectivity
- UDP connectivity
- TLS connectivity

### M1 — Cryptographic foundation
- Curve25519
- ChaCha20-Poly1305
- BLAKE2s
- Secure random generation
- Persistent key material

### M2 — Control plane
- Auth-key registration
- Machine identity
- Node key
- Control-plane API/protocol implementation
- Network-map parsing and storage

### M3 — WireGuard
- WireGuard handshake
- Peer state
- Keepalive
- Encrypted packet send/receive
- Replay protection

### M4 — Tailscale connectivity
- Tailscale addressing
- UDP direct connections
- DISCO discovery
- DERP fallback

### M5 — Usable Arduino library
- `Tailscale.begin()`
- `Tailscale.loop()`
- `Tailscale.connected()`
- status/error reporting
- example sketch
- documentation

## Security constraints

- Never commit auth keys or private keys.
- Avoid hard-coded credentials in examples.
- Validate all control-plane inputs before parsing into persistent state.
- Use constant-time cryptographic operations where required by the primitive.
- Keep private node/machine keys local to the device.

## Definition of success

A physical Teensy 4.1 should register as a node in a Tailscale tailnet using an auth key and, after subsequent milestones, establish an encrypted Tailscale connection with another tailnet node over native Ethernet.
