# Teensy 4.1 Tailscale M0 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Prove the physical Teensy 4.1 native Ethernet path and establish the reusable TCP, UDP, DNS, and TLS transport foundation required by the future Tailscale client.

**Architecture:** M0 is deliberately below the Tailscale protocol layer. The Teensy 4.1 will use its already-wired native Ethernet interface and a maintained Arduino-compatible Ethernet/IP stack. A small transport wrapper will expose link state, IPv4 configuration, DNS, TCP, UDP, and TLS primitives without introducing any Tailscale identity, WireGuard, DERP, or auth-key logic yet.

**Tech Stack:** Teensy 4.1, Arduino/Teensyduino-compatible C++, native Ethernet, lwIP/QNEthernet where supported, Arduino TLS client facilities available for the selected stack, PlatformIO or Arduino IDE build path.

## Global Constraints

- Target hardware is **Teensy 4.1 only** for M0.
- Use the user's physically installed **native Ethernet wiring**; do not add W5500 support.
- Do not implement or fake a Tailscale virtual interface in M0.
- Do not commit a Tailscale auth key, private key, certificate private material, or other credentials.
- M0 must work with DHCP first; static IPv4 is an explicit secondary configuration path.
- Keep the transport layer independent from future WireGuard/control-plane code.
- Prefer existing Teensy/lwIP networking primitives over implementing TCP/IP ourselves.
- Every M0 feature must have a hardware-testable example or deterministic host-side test where hardware is not required.

---

### Task 1: Establish the embedded project structure

**Files:**
- Create: `firmware/teensy41/` project files following the repository's existing conventions after confirming the current tree.
- Create: `firmware/teensy41/src/main.cpp`
- Create: `firmware/teensy41/include/ts_transport.h`
- Create: `firmware/teensy41/src/ts_transport.cpp`
- Create: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino` if the project uses Arduino sketches for examples.

**Interfaces:**
- Produces `TsTransport::begin()`, `TsTransport::loop()`, `TsTransport::connected()`, and network-state accessors for later M1/M2 code.
- The transport object must not know anything about Tailscale keys, WireGuard peers, DERP, or the control plane.

- [ ] **Step 1: Inspect the repository tree and preserve existing unrelated code.**

Run:

```bash
git clone https://github.com/IFindMe/ts.git
cd ts
find . -maxdepth 3 -type f | sort | head -300
```

Confirm the new firmware subtree does not collide with existing project files.

- [ ] **Step 2: Add the smallest compilable Teensy firmware skeleton.**

The initial `main.cpp` should contain only setup/loop and construct the transport object:

```cpp
#include "ts_transport.h"

TsTransport transport;

void setup() {
    transport.begin();
}

void loop() {
    transport.loop();
}
```

- [ ] **Step 3: Define the transport interface.**

Use a minimal interface such as:

```cpp
#pragma once

#include <Arduino.h>

class TsTransport {
public:
    bool begin();
    void loop();
    bool connected() const;
};
```

Do not expose Tailscale-specific types yet.

- [ ] **Step 4: Compile the empty transport skeleton for Teensy 4.1.**

Run the repository's selected Arduino/PlatformIO build command and verify there are no C++ or board-selection errors.

- [ ] **Step 5: Commit the project skeleton.**

```bash
git add firmware/teensy41
git commit -m "feat(teensy): add native ethernet firmware skeleton"
```

---

### Task 2: Bring up native Ethernet and DHCP

**Files:**
- Modify: `firmware/teensy41/include/ts_transport.h`
- Modify: `firmware/teensy41/src/ts_transport.cpp`
- Modify: `firmware/teensy41/src/main.cpp`
- Modify: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino`

**Interfaces:**
- `TsTransport::begin()` initializes the native Ethernet stack and requests DHCP.
- `TsTransport::connected()` reports a usable Ethernet/IP configuration, not merely physical link presence.
- Add read-only accessors for local IPv4, gateway, subnet mask, and DNS server.

- [ ] **Step 1: Add the selected native Ethernet/lwIP dependency to the embedded build.**

Use the Teensy-compatible Ethernet stack that supports the physical native Ethernet interface. Prefer QNEthernet when it is the most stable fit for the selected build environment; do not introduce a W5500 dependency.

- [ ] **Step 2: Implement DHCP startup.**

`begin()` must initialize Ethernet and block only for a bounded startup interval. It must return `false` if no DHCP lease is obtained within that interval rather than hanging forever.

- [ ] **Step 3: Implement non-blocking network maintenance.**

`loop()` must service the networking stack without long delays. Do not use an infinite DHCP wait or `delay()` loop that prevents future WireGuard packet processing.

- [ ] **Step 4: Print deterministic diagnostics.**

The M0 example should report:

```text
[TS] Ethernet initialized
[TS] DHCP: OK
[TS] IP: x.x.x.x
[TS] Gateway: x.x.x.x
[TS] DNS: x.x.x.x
[TS] Link: up
```

Errors must be explicit and machine-readable enough to diagnose hardware versus DHCP failure.

- [ ] **Step 5: Flash the physical Teensy and verify DHCP.**

Acceptance test:

```text
Teensy powers on
→ Ethernet link becomes active
→ DHCP lease is obtained
→ printed IPv4 address matches the LAN lease
→ gateway and DNS are populated
→ firmware remains responsive
```

- [ ] **Step 6: Commit the working Ethernet/DHCP milestone.**

```bash
git add firmware/teensy41
git commit -m "feat(teensy): bring up native ethernet with dhcp"
```

---

### Task 3: Add bounded DNS and TCP connectivity tests

**Files:**
- Create: `firmware/teensy41/include/ts_tcp.h`
- Create: `firmware/teensy41/src/ts_tcp.cpp`
- Modify: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino`

**Interfaces:**
- `TsTcpClient::connect(host, port, timeout_ms)`.
- `TsTcpClient::write(...)` and `read(...)` sufficient for a small test request.
- DNS resolution must use the configured network resolver and have a bounded timeout.

- [ ] **Step 1: Define a tiny TCP client abstraction around the selected Ethernet stack.**

Keep socket ownership and timeout behavior inside `TsTcpClient`; do not leak stack-specific socket types into future Tailscale code.

- [ ] **Step 2: Add DNS resolution.**

Resolve a known test hostname using the DHCP-provided DNS server. The test must fail cleanly if DNS is unavailable.

- [ ] **Step 3: Add a TCP connectivity test.**

Connect to a configurable host/port. Default the example to a harmless public HTTPS endpoint's TCP port only for connection testing; do not require a successful HTTP application response yet.

- [ ] **Step 4: Test LAN and Internet paths separately.**

Acceptance:

```text
LAN TCP endpoint reachable
AND
Internet TCP endpoint reachable when Internet is available
```

A missing Internet connection must not be reported as a Teensy Ethernet hardware failure if DHCP and LAN connectivity still work.

- [ ] **Step 5: Commit.**

```bash
git add firmware/teensy41
 git commit -m "feat(teensy): add dns and tcp transport tests"
```

---

### Task 4: Add UDP transport and packet loopback

**Files:**
- Create: `firmware/teensy41/include/ts_udp.h`
- Create: `firmware/teensy41/src/ts_udp.cpp`
- Modify: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino`

**Interfaces:**
- `TsUdp::begin(local_port)`.
- `TsUdp::send(remote_ip, remote_port, data, length)`.
- `TsUdp::poll(...)` for non-blocking receive.

- [ ] **Step 1: Define a fixed-size, allocation-free UDP packet path for M0.**

Use caller-provided buffers and explicit lengths. Avoid dynamic allocation in the packet receive path.

- [ ] **Step 2: Implement UDP send/receive.**

Wrap the selected Ethernet stack's UDP API without exposing it to future protocol layers.

- [ ] **Step 3: Add a LAN UDP echo test.**

The example sends a short test packet to a configurable host and validates the echoed payload and length.

- [ ] **Step 4: Verify sustained polling.**

The example must continue servicing UDP packets while diagnostics are printed, proving that the networking loop is non-blocking enough for future WireGuard traffic.

- [ ] **Step 5: Commit.**

```bash
git add firmware/teensy41
 git commit -m "feat(teensy): add udp transport"
```

---

### Task 5: Establish TLS capability for the future control plane

**Files:**
- Create: `firmware/teensy41/include/ts_tls.h`
- Create: `firmware/teensy41/src/ts_tls.cpp`
- Modify: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino`
- Create: `firmware/teensy41/certs/README.md`

**Interfaces:**
- `TsTlsClient::connect(host, port, timeout_ms)`.
- `TsTlsClient::write(...)` and bounded `read(...)`.
- Certificate validation must be enabled for the actual control-plane client; M0 may use an explicit test configuration only when documented.

- [ ] **Step 1: Identify the TLS implementation actually available for the chosen Teensy Ethernet stack and board package.**

Do not assume an ESP32-specific TLS API is portable to Teensy 4.1. Confirm the headers, constructors, certificate APIs, and memory requirements against the installed Teensy toolchain.

- [ ] **Step 2: Implement the smallest TLS client wrapper.**

Keep the wrapper independent from HTTP and Tailscale. It should provide connection, certificate validation configuration, read, write, and close operations.

- [ ] **Step 3: Add a real HTTPS handshake test.**

The test should connect to a configurable HTTPS hostname and report whether DNS, TCP, TLS handshake, and certificate verification succeeded separately.

- [ ] **Step 4: Measure memory usage during TLS.**

Record free RAM before and after the TLS handshake and while holding the connection. This measurement is important because later Tailscale/WireGuard state must coexist with TLS.

- [ ] **Step 5: Document certificate provisioning without committing credentials.**

`certs/README.md` must state where test CA/root material is supplied during development and explicitly prohibit committing private keys or auth keys.

- [ ] **Step 6: Commit.**

```bash
git add firmware/teensy41
 git commit -m "feat(teensy): add tls transport foundation"
```

---

### Task 6: Add M0 hardware test procedure and resource baseline

**Files:**
- Create: `firmware/teensy41/tests/M0-HARDWARE.md`
- Modify: `firmware/teensy41/examples/m0_ethernet/m0_ethernet.ino`
- Modify: `README.md` only if the repository already has a user-facing README; otherwise leave the existing project layout untouched until the repository's documentation structure is understood.

**Interfaces:**
- Produces a repeatable physical test procedure for the exact Teensy 4.1 native Ethernet wiring.
- Produces baseline RAM/flash/network observations needed before M1.

- [ ] **Step 1: Document physical prerequisites.**

Include Teensy 4.1, the user's soldered native Ethernet connection, Ethernet cable, DHCP-capable LAN, and USB serial connection.

- [ ] **Step 2: Document the test sequence.**

The sequence must be:

```text
boot
→ Ethernet link
→ DHCP
→ DNS
→ TCP
→ UDP
→ TLS
```

Each stage must have a clear pass/fail condition.

- [ ] **Step 3: Record resource baseline.**

Print and document at minimum:

```text
firmware size
free RAM at boot
free RAM after Ethernet
free RAM after TLS handshake
UDP packet size successfully transmitted/received
```

- [ ] **Step 4: Verify failure behavior.**

Test at least:

```text
Ethernet cable disconnected
DHCP unavailable
DNS unavailable
TCP destination unavailable
TLS certificate invalid
```

The device must recover or report failure without locking the main loop indefinitely.

- [ ] **Step 5: Commit.**

```bash
git add firmware/teensy41
 git commit -m "docs(teensy): document m0 hardware validation"
```

---

## M0 Acceptance Criteria

M0 is complete only when the physical Teensy 4.1 can demonstrate all of the following over its native Ethernet interface:

1. Ethernet link detection works.
2. DHCP obtains a valid IPv4 configuration.
3. DNS resolution succeeds.
4. TCP connection succeeds to a reachable endpoint.
5. UDP send/receive succeeds against a LAN test peer.
6. TLS handshake and certificate verification work with the selected Teensy-compatible TLS stack.
7. The main loop remains responsive during networking operations.
8. Failure cases are bounded and diagnostic.
9. No Tailscale auth key or private key is stored in the repository.
10. RAM/flash baseline is recorded so M1 cryptographic memory requirements can be evaluated against actual hardware.

M0 does **not** claim Tailscale connectivity. It only proves the transport substrate required for the later Tailscale implementation.
