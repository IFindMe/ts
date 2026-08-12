#include <Arduino.h>
#include <QNEthernet.h>

using namespace qindesign::network;

EthernetUDP udp;
EthernetClient tcp;

static void printAddress(const char *label, const IPAddress &address) {
  Serial.print(label);
  Serial.println(address);
}

static bool waitForDhcp(uint32_t timeoutMs) {
  const uint32_t start = millis();
  while (Ethernet.localIP() == INADDR_NONE && millis() - start < timeoutMs) {
    Ethernet.maintain();
    delay(100);
  }
  return Ethernet.localIP() != INADDR_NONE;
}

static bool testDns() {
  IPAddress address;
  if (!Ethernet.hostByName("example.com", address)) {
    Serial.println("[FAIL] DNS lookup example.com");
    return false;
  }
  Serial.print("[PASS] DNS example.com -> ");
  Serial.println(address);
  return true;
}

static bool testTcp() {
  Serial.println("[TEST] TCP example.com:80");
  if (!tcp.connect("example.com", 80)) {
    Serial.println("[FAIL] TCP connection");
    return false;
  }

  tcp.println("GET / HTTP/1.1");
  tcp.println("Host: example.com");
  tcp.println("Connection: close");
  tcp.println();

  const uint32_t start = millis();
  bool received = false;
  while (millis() - start < 5000) {
    Ethernet.maintain();
    if (tcp.available()) {
      received = true;
      break;
    }
    delay(10);
  }

  tcp.stop();
  Serial.println(received ? "[PASS] TCP data received" : "[FAIL] TCP data timeout");
  return received;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Tailscale Teensy 4.1 / M0 ===");
  Serial.println("Native Ethernet diagnostic");

  Ethernet.begin();

  const uint32_t linkStart = millis();
  while (!Ethernet.linkStatus() && millis() - linkStart < 10000) {
    delay(100);
  }

  Serial.println(Ethernet.linkStatus()
                     ? "[PASS] Ethernet link detected"
                     : "[FAIL] Ethernet link not detected");

  if (!waitForDhcp(20000)) {
    Serial.println("[FAIL] DHCP did not provide an IPv4 address");
  } else {
    Serial.println("[PASS] IPv4 address acquired");
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

  if (Ethernet.localIP() != INADDR_NONE) {
    testDns();
    testTcp();
  }

  Serial.println("M0 network validation complete.");
}

void loop() {
  Ethernet.maintain();
  delay(10);
}
