#pragma once

#include <stddef.h>
#include <stdint.h>

namespace ts {

class TsIdentity {
 public:
  static constexpr size_t kKeySize = 32;
  static constexpr size_t kMaxAuthKeySize = 128;

  bool begin();
  bool initialized() const;

  const uint8_t *machinePrivate() const;
  const uint8_t *nodePrivate() const;

  bool machinePublic(uint8_t out[kKeySize]) const;
  bool nodePublic(uint8_t out[kKeySize]) const;

  bool setAuthKey(const char *authKey, size_t length);
  bool hasAuthKey() const;
  const char *authKey() const;
  void clearAuthKey();

  bool reset();

 private:
  bool initialized_ = false;
  uint8_t machinePrivate_[kKeySize] = {};
  uint8_t nodePrivate_[kKeySize] = {};
  char authKey_[kMaxAuthKeySize + 1] = {};
};

}  // namespace ts
