#include "../managed_string.h"

struct managed_string {
  const uint8_t *__restrict__ data;
  size_t strlen;
  uint64_t rct;
  bool is_utf8;
  bool needs_san;
  bool lock;
  uint8_t padding0;
};
