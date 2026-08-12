#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "blake2s.h"

static bool expectHex(const uint8_t *actual, const char *expected) {
  char hex[65] = {};
  for (size_t i = 0; i < 32; ++i) {
    std::snprintf(hex + i * 2, 3, "%02x", actual[i]);
  }
  return std::strcmp(hex, expected) == 0;
}

int main() {
  uint8_t digest[32];
  const uint8_t abc[] = {'a', 'b', 'c'};

  if (ts_blake2s(digest, 32, nullptr, 0, nullptr, 0) != 0 ||
      !expectHex(digest, "69217a3079908094e11121d042354a7c1f55b6482ca1a51e1b250dfd1ed0eef9")) {
    std::fprintf(stderr, "BLAKE2s empty vector failed\n");
    return 1;
  }

  if (ts_blake2s(digest, 32, nullptr, 0, abc, sizeof(abc)) != 0 ||
      !expectHex(digest, "508c5e8c327c14e2e1a72ba34eeb452f37458b209ed63a294d999b4c86675982")) {
    std::fprintf(stderr, "BLAKE2s abc vector failed\n");
    return 2;
  }

  const uint8_t key[] = {'k', 'e', 'y'};
  if (ts_blake2s(digest, 32, key, sizeof(key), abc, sizeof(abc)) != 0 ||
      !expectHex(digest, "3f9723437b033bf0c1f4df43cafd0776068cb0a95912de13f3b2952a3aba764d")) {
    std::fprintf(stderr, "BLAKE2s keyed vector failed\n");
    return 3;
  }

  std::puts("BLAKE2s host tests: PASS");
  return 0;
}
