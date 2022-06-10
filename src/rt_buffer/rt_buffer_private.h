#pragma once

#ifndef _RTHOST_STRING_PRIVATE_
#define _RTHOST_STRING_PRIVATE_

#include "rt_buffer.h"

struct managed_buffer {
  uint8_t *data;
  size_t len;
  uint64_t ref_ct;
  bool is_utf8;
  bool needs_sync;
  bool lock;
  uint8_t padding0;
};

#endif