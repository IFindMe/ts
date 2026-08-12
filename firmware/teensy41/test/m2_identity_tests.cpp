#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "ts_auth.h"
#include "ts_state.h"

int main() {
  const char *valid = "tskey-auth-0123456789abcdef";
  assert(ts::auth::isValidAuthKey(valid, std::strlen(valid)));

  const char *wrongPrefix = "tskey-api-0123456789abcdef";
  assert(!ts::auth::isValidAuthKey(wrongPrefix, std::strlen(wrongPrefix)));
  assert(!ts::auth::isValidAuthKey("", 0));

  char longKey[130] = {};
  std::memcpy(longKey, "tskey-auth-", 11);
  for (size_t i = 11; i < sizeof(longKey) - 1; ++i) longKey[i] = 'a';
  assert(!ts::auth::isValidAuthKey(longKey, sizeof(longKey) - 1));

  const uint8_t vector[] = {'1','2','3','4','5','6','7','8','9'};
  assert(ts::state::crc32(vector, sizeof(vector)) == 0xcbf43926u);

  return 0;
}
