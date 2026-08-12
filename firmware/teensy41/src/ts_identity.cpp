#include "ts_identity.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <cstring>

#include "ts_auth.h"
#include "ts_crypto.h"
#include "ts_state.h"

namespace ts {
namespace {

constexpr uint32_t kMagic = 0x54534D32u;  // "TSM2"
constexpr uint16_t kVersion = 1;
constexpr int kEepromAddress = 0;

struct StoredState {
  uint32_t magic;
  uint16_t version;
  uint16_t reserved;
  uint8_t machinePrivate[32];
  uint8_t nodePrivate[32];
  uint32_t crc;
};

uint32_t stateCrc(const StoredState &state) {
  return ts::state::crc32(reinterpret_cast<const uint8_t *>(&state),
                           offsetof(StoredState, crc));
}

bool validState(const StoredState &state) {
  return state.magic == kMagic && state.version == kVersion &&
         stateCrc(state) == state.crc;
}

void wipe(void *ptr, size_t length) {
  volatile uint8_t *p = static_cast<volatile uint8_t *>(ptr);
  while (length--) *p++ = 0;
}

bool loadState(StoredState &state) {
  EEPROM.get(kEepromAddress, state);
  return validState(state);
}

bool saveState(const StoredState &state) {
  EEPROM.put(kEepromAddress, state);
  StoredState check{};
  EEPROM.get(kEepromAddress, check);
  return validState(check) &&
         std::memcmp(check.machinePrivate, state.machinePrivate, 32) == 0 &&
         std::memcmp(check.nodePrivate, state.nodePrivate, 32) == 0;
}

bool generateState(StoredState &state) {
  state = {};
  state.magic = kMagic;
  state.version = kVersion;

  if (!ts::crypto::randomBytes(state.machinePrivate, 32) ||
      !ts::crypto::randomBytes(state.nodePrivate, 32)) {
    return false;
  }

  state.crc = stateCrc(state);
  return saveState(state);
}

}  // namespace

bool TsIdentity::begin() {
  StoredState state{};
  if (!loadState(state)) {
    if (!generateState(state)) {
      initialized_ = false;
      return false;
    }
  }

  std::memcpy(machinePrivate_, state.machinePrivate, sizeof(machinePrivate_));
  std::memcpy(nodePrivate_, state.nodePrivate, sizeof(nodePrivate_));
  initialized_ = true;
  return true;
}

bool TsIdentity::initialized() const { return initialized_; }

const uint8_t *TsIdentity::machinePrivate() const { return machinePrivate_; }
const uint8_t *TsIdentity::nodePrivate() const { return nodePrivate_; }

bool TsIdentity::machinePublic(uint8_t out[kKeySize]) const {
  return initialized_ && ts::crypto::x25519PublicKey(out, machinePrivate_);
}

bool TsIdentity::nodePublic(uint8_t out[kKeySize]) const {
  return initialized_ && ts::crypto::x25519PublicKey(out, nodePrivate_);
}

bool TsIdentity::setAuthKey(const char *authKey, size_t length) {
  if (!ts::auth::isValidAuthKey(authKey, length) ||
      length > kMaxAuthKeySize) {
    return false;
  }
  clearAuthKey();
  std::memcpy(authKey_, authKey, length);
  authKey_[length] = '\0';
  return true;
}

bool TsIdentity::hasAuthKey() const { return authKey_[0] != '\0'; }
const char *TsIdentity::authKey() const { return hasAuthKey() ? authKey_ : nullptr; }

void TsIdentity::clearAuthKey() { wipe(authKey_, sizeof(authKey_)); }

bool TsIdentity::reset() {
  StoredState state{};
  if (!generateState(state)) {
    return false;
  }
  clearAuthKey();
  wipe(machinePrivate_, sizeof(machinePrivate_));
  wipe(nodePrivate_, sizeof(nodePrivate_));
  std::memcpy(machinePrivate_, state.machinePrivate, sizeof(machinePrivate_));
  std::memcpy(nodePrivate_, state.nodePrivate, sizeof(nodePrivate_));
  initialized_ = true;
  return true;
}

}  // namespace ts
