#pragma once

#include <stddef.h>

namespace ts::auth {

constexpr size_t kMaxAuthKeySize = 128;

bool isValidAuthKey(const char *key, size_t length);

}  // namespace ts::auth
