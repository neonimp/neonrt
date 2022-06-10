/**
 * @file NRT_wireutils.h
 * @brief Utilities for encoding/decoding integers to and from bytes in a stream little or big endian
 * @authors Matheus Xavier
 * This file is part of BareSDK and licensed under the:
 * BSD 3-Clause License
 * Copyright (c) 2021, Matheus Xavier
 * All rights reserved.
 */

#ifndef NRT_WIRE_H
#define NRT_WIRE_H

#include <stdint.h>
#include <stddef.h>
#include <endian.h>

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#define BE_ARCH
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#define LE_ARCH
#else
#error Unable to determine arch
#endif

#define ALIGN(value, alignment) (((value) + (alignment - 1)) & ~(alignment - 1))

#define NRT_READ16LE(S) ((255 & (S)[1]) << 8 | (255 & (S)[0]))
#define NRT_READ16BE(S) ((255 & (S)[0]) << 8 | (255 & (S)[1]))
#define NRT_READ32LE(S) \
	((uint32_t)(255 & (S)[3]) << 030 | (uint32_t)(255 & (S)[2]) << 020 | (uint32_t)(255 & (S)[1]) << 010 | (uint32_t)(255 & (S)[0]) << 000)
#define NRT_READ32BE(S) \
	((uint32_t)(255 & (S)[0]) << 030 | (uint32_t)(255 & (S)[1]) << 020 | (uint32_t)(255 & (S)[2]) << 010 | (uint32_t)(255 & (S)[3]) << 000)
#define NRT_READ64LE(S) \
	((uint64_t)(255 & (S)[7]) << 070 | (uint64_t)(255 & (S)[6]) << 060 | (uint64_t)(255 & (S)[5]) << 050 | (uint64_t)(255 & (S)[4]) << 040 | (uint64_t)(255 & (S)[3]) << 030 | (uint64_t)(255 & (S)[2]) << 020 | (uint64_t)(255 & (S)[1]) << 010 | (uint64_t)(255 & (S)[0]) << 000)
#define NRT_READ64BE(S) \
	((uint64_t)(255 & (S)[0]) << 070 | (uint64_t)(255 & (S)[1]) << 060 | (uint64_t)(255 & (S)[2]) << 050 | (uint64_t)(255 & (S)[3]) << 040 | (uint64_t)(255 & (S)[4]) << 030 | (uint64_t)(255 & (S)[5]) << 020 | (uint64_t)(255 & (S)[6]) << 010 | (uint64_t)(255 & (S)[7]) << 000)

#define NRT_WRITE16LE(P, V)                         \
	((P)[0] = (0x00000000000000FF & (V)) >> 000, \
	 (P)[1] = (0x000000000000FF00 & (V)) >> 010, (P) + 2)
#define NRT_WRITE16BE(P, V)                         \
	((P)[0] = (0x000000000000FF00 & (V)) >> 010, \
	 (P)[1] = (0x00000000000000FF & (V)) >> 000, (P) + 2)
#define NRT_WRITE32LE(P, V)                         \
	((P)[0] = (0x00000000000000FF & (V)) >> 000, \
	 (P)[1] = (0x000000000000FF00 & (V)) >> 010, \
	 (P)[2] = (0x0000000000FF0000 & (V)) >> 020, \
	 (P)[3] = (0x00000000FF000000 & (V)) >> 030, (P) + 4)
#define NRT_WRITE32BE(P, V)                         \
	((P)[0] = (0x00000000FF000000 & (V)) >> 030, \
	 (P)[1] = (0x0000000000FF0000 & (V)) >> 020, \
	 (P)[2] = (0x000000000000FF00 & (V)) >> 010, \
	 (P)[3] = (0x00000000000000FF & (V)) >> 000, (P) + 4)
#define NRT_WRITE64LE(P, V)                         \
	((P)[0] = (0x00000000000000FF & (V)) >> 000, \
	 (P)[1] = (0x000000000000FF00 & (V)) >> 010, \
	 (P)[2] = (0x0000000000FF0000 & (V)) >> 020, \
	 (P)[3] = (0x00000000FF000000 & (V)) >> 030, \
	 (P)[4] = (0x000000FF00000000 & (V)) >> 040, \
	 (P)[5] = (0x0000FF0000000000 & (V)) >> 050, \
	 (P)[6] = (0x00FF000000000000 & (V)) >> 060, \
	 (P)[7] = (0xFF00000000000000 & (V)) >> 070, (P) + 8)
#define NRT_WRITE64BE(P, V)                         \
	((P)[0] = (0xFF00000000000000 & (V)) >> 070, \
	 (P)[1] = (0x00FF000000000000 & (V)) >> 060, \
	 (P)[2] = (0x0000FF0000000000 & (V)) >> 050, \
	 (P)[3] = (0x000000FF00000000 & (V)) >> 040, \
	 (P)[4] = (0x00000000FF000000 & (V)) >> 030, \
	 (P)[5] = (0x0000000000FF0000 & (V)) >> 020, \
	 (P)[6] = (0x000000000000FF00 & (V)) >> 010, \
	 (P)[7] = (0x00000000000000FF & (V)) >> 000, (P) + 8)

/*
 * These methods are simply implementations of the macros as functions
 * that can be loaded and called at runtime.
 */

/**
 * @brief Reads 2 bytes of the buffer as an uint16_t in little endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint16_t read16le(void *buffer);

/**
 * @brief Reads 2 bytes of the buffer as an uint16_t in big endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint16_t read16be(void *buffer);

/**
 * @brief Reads 4 bytes of the buffer as an uint32_t in little endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint32_t read32le(void *buffer);

/**
 * @brief Reads 4 bytes of the buffer as an uint32_t in big endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint32_t read32be(void *buffer);

/**
 * @brief Reads 8 bytes of the buffer as an uint64_t in little endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint64_t read64le(void *buffer);

/**
 * @brief Reads 8 bytes of the buffer as an uint64_t in big endian
 * order from the buffer.
 * @param buffer
 * @return the read value
 */
extern uint64_t read64be(void *buffer);

/**
 * @brief Writes value as 2 bytes in little endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write16le(void *buffer, uint16_t value);

/**
 * @brief Writes value as 2 bytes in big endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write16be(void *buffer, uint16_t value);

/**
 * @brief Writes value as 4 bytes in little endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write32le(void *buffer, uint32_t value);

/**
 * @brief Writes value as 4 bytes in big endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write32be(void *buffer, uint32_t value);

/**
 * @brief Writes value as 8 bytes in little endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write64le(void *buffer, uint64_t value);

/**
 * @brief Writes value as 8 bytes in big endian order to buffer.
 * @param buffer
 * @param value
 */
extern void write64be(void *buffer, uint64_t value);

extern size_t align(size_t value, uint8_t alignment);

#endif
