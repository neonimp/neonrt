#pragma once

#ifndef _RT_HOST_SIP_
#define _RT_HOST_SIP_

#include <stdint.h>

extern uint64_t wrap_siphash(const void *data, size_t data_len, uint64_t seed);

#endif // _RT_HOST_SIP_
