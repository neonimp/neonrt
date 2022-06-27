#include "secure_erase.h"

__attribute__((weak)) void __explicit_erase_hook(void *buf, size_t len)
{
	(void)buf;
	(void)len;
}

void explicit_erase(void *buf, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		((uint8_t *)buf)[i] = 0;
	}
	__explicit_erase_hook(buf, len);
}
