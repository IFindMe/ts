#include <Arduino.h>

#include "ts_identity.h"

TsIdentity identity;

static void printHex(const char *label, const uint8_t *data, size_t length) {
  Serial.print(label);
  for (size_t i = 0; i < length; ++i) {
    if (data[i] < 0x10) Serial.print('0');
    Serial.print(data[i], HEX);
  }
  Serial.println();
}

static void printPublicKeys() {
  uint8_t machinePublic[32];
  uint8_t nodePublic[32];

  if (!identity.machinePublic(machinePublic) || !identity.nodePublic(nodePublic)) {
    Serial.println("[FAIL] public-key derivation");
    return;
  }

  printHex("machine-public=", machinePublic, sizeof(machinePublic));
  printHex("node-public=", nodePublic, sizeof(nodePublic));
}

static void handleCommand(String line) {
  line.trim();
  if (line.startsWith("AUTH ")) {
    const char *value = line.c_str() + 5;
    const size_t length = line.length() - 5;
    Serial.println(identity.setAuthKey(value, length)
                       ? "AUTH KEY ACCEPTED"
                       : "AUTH KEY REJECTED");
    return;
  }

  if (line == "CLEAR") {
    identity.clearAuthKey();
    Serial.println("AUTH KEY CLEARED");
    return;
  }

  if (line == "KEYS") {
    printPublicKeys();
    return;
  }

  if (line == "RESET") {
    Serial.println(identity.reset() ? "IDENTITY RESET" : "IDENTITY RESET FAILED");
    if (identity.initialized()) printPublicKeys();
    return;
  }

  Serial.println("COMMANDS: AUTH <tskey-auth-...> | CLEAR | KEYS | RESET");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Tailscale Teensy 4.1 / M2 ===");
  Serial.println("Persistent machine/node identity");

  if (!identity.begin()) {
    Serial.println("[FAIL] identity initialization");
    return;
  }

  Serial.println("[PASS] identity initialized");
  printPublicKeys();
  Serial.println("AUTH key is RAM-only; M3 will consume it for registration.");
}

void loop() {
  if (Serial.available()) {
    handleCommand(Serial.readStringUntil('\n'));
  }
}
