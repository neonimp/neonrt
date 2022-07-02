#pragma once

#ifndef _NEON_SECURE_ERASE_
#define _NEON_SECURE_ERASE_

#include <stdint.h>
#include <stddef.h>

extern void explicit_erase(void *buf, size_t len);

#endif // _NEON_SECURE_ERASE_
