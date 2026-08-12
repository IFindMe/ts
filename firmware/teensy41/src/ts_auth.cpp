#include "ts_auth.h"

#include <cstring>

namespace ts::auth {

bool isValidAuthKey(const char *key, size_t length) {
  constexpr char prefix[] = "tskey-auth-";
  constexpr size_t prefixLength = sizeof(prefix) - 1;

  if (key == nullptr || length <= prefixLength || length > kMaxAuthKeySize) {
    return false;
  }
  return std::memcmp(key, prefix, prefixLength) == 0;
}

}  // namespace ts::auth
