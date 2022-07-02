#pragma once

#ifndef NEON_SIP
#define NEON_SIP

#include <stdint.h>
#include <stddef.h>

extern uint64_t wrap_siphash(const void *data, size_t data_len, uint64_t seed);

#endif // NEON_SIP
