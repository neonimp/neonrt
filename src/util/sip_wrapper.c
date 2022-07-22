#include "sip_wrapper.h"
#include "../extra/siphash.h"
#include "wire.h"

uint64_t wrap_siphash(const void *data, size_t data_len, uint64_t seed)
{
	uint32_t val;
	uint8_t key[32];
	uint8_t hash[8];

	write64be(key, seed);
	write64le(key + 8, seed);
	val = read32be(key);
	val = val ^ (val << 8);
	write32be(key, val);
	siphash(data, data_len, key, hash, 8);
	return read64be(hash);
}
