# M0 acceptance checks

M0 is hardware validation, so the final acceptance test runs on a physical Teensy 4.1.

## Required checks

- [ ] Native Ethernet link comes up.
- [ ] DHCP obtains an IPv4 address, or documented static configuration works.
- [ ] DNS lookup succeeds.
- [ ] TCP connection succeeds.
- [ ] UDP send/receive succeeds.
- [ ] TLS connection succeeds with certificate validation.
- [ ] Device remains responsive while `loop()` services networking.
- [ ] Serial output reports failures without exposing credentials.

## Test discipline

Each check must have an observable pass/fail result in the serial log. Do not add Tailscale protocol code until the M0 network foundation is verified on hardware.
