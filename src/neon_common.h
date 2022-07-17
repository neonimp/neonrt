#pragma once

#ifndef NEON_SRC_RT_COMMON_H
#define NEON_SRC_RT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include <stdio.h>
#include "util/sip_wrapper.h"

#define rt_hash_fn(ptr, len) wrap_siphash(ptr, len, 0x19E3779B97F4A7C1)
#define rt_malloc_fn(size) malloc(size)
#define rt_free_fn(ptr) free(ptr)
#define rt_realloc_fn(ptr, size) realloc(ptr, size)
#define rt_calloc_fn(count, size) calloc(count, size)
#define rt_memalign_fn(alignment, size) memalign(alignment, size)
#define rt_sprintf_fn(str, fmt, ...) sprintf(str, fmt, ##__VA_ARGS__)

#endif // NEON_SRC_RT_COMMON_H
