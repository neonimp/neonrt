#pragma once

#ifndef _RTHOST_SECURE_ERASE_
#define _RTHOST_SECURE_ERASE_

#include <stdint.h>
#include <stddef.h>

extern void explicit_erase(void *buf, size_t len);

#endif // _RTHOST_SECURE_ERASE_
