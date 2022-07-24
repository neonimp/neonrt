/**
 * @brief Strings that are more than a byte array
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef NEON_SRC_CONTAINERS_BUFFER_H
#define NEON_SRC_CONTAINERS_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../neon_common.h"
#include <utf8proc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory.h>

struct managed_buffer;

typedef struct managed_buffer neon_buff_t;

/**
 * @brief Create a new managed string from a buffer of bytes.
 * @param value Buffer to initialize the managed string with.
 * @param val_len Length of the buffer `value`.
 * @return A pointer to an initialized neon_buff_t instance.
 */
extern neon_buff_t *buff_new(const uint8_t *value, size_t val_len);

/**
 * @brief Create a new managed buffer with strict UTF-8 validation.
 * @param value Buffer to initialize the managed string with.
 * @param val_len Length of the buffer `value`.
 * @return A pointer to an initialized neon_buff_t instance.
 */
extern neon_buff_t *buff_new_utf8(const uint8_t *value, size_t val_len);

/**
 * @brief Get a reference to the underlying buffer of a managed string to use
 * with functions that need a raw const char pointer.
 * @attention Never pass this pointer to a function that modifies the buffer
 * in place, as this cause for UNDEFINED BEHAVIOUR, since the properties of the
 * buffer are tracked by the internal state.
 * @param self Reference to the managed string.
 * @return A reference to the underlying buffer.
 */
extern const char *buff_borrow(neon_buff_t *self);

/**
 * @brief Get a write-able reference to the underlying buffer of a managed
 * string to use with functions that need a raw const char pointer, note that
 * the write-able reference MUST BE the only reference, this function will fail
 * if ANY of the following is true:
 * There is already ANY references to the buffer leased out.
 * There is a lock on the string
 * @attention Never pass this pointer to a function that modifies the buffer
 * in place, as this cause for UNDEFINED BEHAVIOUR, since the properties of the
 * buffer are tracked by the internal state.
 * @param self Reference to the managed string.
 * @return A reference to the underlying buffer.
 */
extern char *buff_borrow_writable(neon_buff_t *self);

/**
 * @brief Returns a borrowed reference to the underlying buffer and decreases
 * the reference counter.
 * @param self Reference to the managed string.
 */
extern void buff_return(neon_buff_t *self);

/**
 * @brief Return n borrowed references back.
 * @param self Reference to the managed string.
 * @param n Number of references to return.
 * @return True if the references were returned, false if there were not enough
 */
extern bool buff_return_n(neon_buff_t *self, size_t n);

/**
 * @brief Get the size of the underlying buffer
 * @param self Buffer to get the size of
 * @return
 */
extern size_t buff_len(const neon_buff_t *self);

/**
 * @brief Compare two managed buffers.
 * @param left Reference to the left managed buffer.
 * @param right Reference to the right managed buffer.
 * @return true if the buffers are equal, false otherwise.
 */
extern bool buff_cmp(const neon_buff_t *left, const neon_buff_t *right);

/**
 * @brief Compare a managed buffer with a raw buffer.
 * @param left Reference to the left managed buffer.
 * @param right Reference to the right raw buffer.
 * @param len Length of the raw buffer.
 * @return true if the buffers are equal, false otherwise.
 */
extern bool buff_cmp_raw(const neon_buff_t *left, const uint8_t *right, size_t len);

/**
 * @brief Destroys a managed string if it's reference counter is 0, and there
 * is no lock on it, do not use a reference to a string passed to this function
 * after calling it.
 * @param self The string to attempt destruction on.
 * @return 0 if the string was deallocated successfully
 * or the refcount otherwise, check the result!.
 */
extern uint64_t buff_free(neon_buff_t *self);

/**
 * @brief Force free a managed buffer, even if it's reference counter is not 0,
 * and there is no lock on it, do not use a reference to a buffer passed to this,
 * function after calling it. Useful ensure that a buffer is deallocated even if
 * it's reference counter is not 0, at the exit of a program.
 * @param self The string to attempt destruction on.
 * @return 0 if the string was deallocated successfully
 * @warning This function is not thread safe.
 * @warning This function can cause dangling pointers.
 */
extern uint64_t buff_force_free(neon_buff_t *self);

#define buff_from_cstr(str, len) buff_new((const uint8_t *)(str), len)
#define buff_new_from_raw(str, len) buff_new((const uint8_t *)(str), len)

#ifdef __cplusplus
}
#endif

#endif // NEON_SRC_CONTAINERS_BUFFER_H
