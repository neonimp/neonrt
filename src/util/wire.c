/**
 * This file is part of BareSDK and licensed under the:
 * BSD 3-Clause License
 * Copyright (c) 2021, Matheus Xavier
 * All rights reserved.
*/

#include "wire.h"

uint16_t read16le(void *buffer)
{
	return NEON_READ16LE((uint8_t *)buffer);
}

uint16_t read16be(void *buffer)
{
	return NEON_READ16BE((uint8_t *)buffer);
}

uint32_t read32le(void *buffer)
{
	return NEON_READ32LE((uint8_t *)buffer);
}

uint32_t read32be(void *buffer)
{
	return NEON_READ32BE((uint8_t *)buffer);
}

uint64_t read64le(void *buffer)
{
	return NEON_READ64LE((uint8_t *)buffer);
}

uint64_t read64be(void *buffer)
{
	return NEON_READ64BE((uint8_t *)buffer);
}

void write16le(void *buffer, uint16_t value)
{
	NEON_WRITE16LE((uint8_t *)buffer, value);
}

void write16be(void *buffer, uint16_t value)
{
	NEON_WRITE16BE((uint8_t *)buffer, value);
}

void write32le(void *buffer, uint32_t value)
{
	NEON_WRITE32LE((uint8_t *)buffer, value);
}

void write32be(void *buffer, uint32_t value)
{
	NEON_WRITE32BE((uint8_t *)buffer, value);
}

void write64le(void *buffer, uint64_t value)
{
	NEON_WRITE64LE((uint8_t *)buffer, value);
}

void write64be(void *buffer, uint64_t value)
{
	NEON_WRITE64BE((uint8_t *)buffer, value);
}

size_t align(size_t value, uint8_t alignment)
{
	return ALIGN(value, alignment);
}
