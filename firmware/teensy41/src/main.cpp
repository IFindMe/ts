#include <Arduino.h>
#include <QNEthernet.h>

using namespace qindesign::network;

EthernetUDP udp;

static void printAddress(const char *label, const IPAddress &address) {
  Serial.print(label);
  Serial.println(address);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Tailscale Teensy 4.1 / M0 ===");
  Serial.println("Native Ethernet diagnostic");

  Ethernet.begin();

  const uint32_t start = millis();
  while (!Ethernet.linkStatus() && millis() - start < 10000) {
    delay(100);
  }

  if (Ethernet.linkStatus()) {
    Serial.println("[PASS] Ethernet link detected");
  } else {
    Serial.println("[FAIL] Ethernet link not detected");
  }

  const uint32_t dhcpStart = millis();
  while (Ethernet.localIP() == INADDR_NONE && millis() - dhcpStart < 20000) {
    Ethernet.maintain();
    delay(100);
  }

  if (Ethernet.localIP() != INADDR_NONE) {
    Serial.println("[PASS] IPv4 address acquired");
  } else {
    Serial.println("[FAIL] DHCP did not provide an IPv4 address");
  }

  printAddress("IP:   ", Ethernet.localIP());
  printAddress("Mask: ", Ethernet.subnetMask());
  printAddress("GW:   ", Ethernet.gatewayIP());
  printAddress("DNS:  ", Ethernet.dnsServerIP());

  if (udp.begin(0)) {
    Serial.println("[PASS] UDP socket initialized");
    udp.stop();
  } else {
    Serial.println("[FAIL] UDP socket initialization");
  }

  Serial.println("M0 hardware test ready.");
}

void loop() {
  Ethernet.maintain();
  delay(10);
}
