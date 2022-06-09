/**
 * @brief Strings that are more than a byte array
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef _RTHOST_STRING_
#define _RTHOST_STRING_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct managed_buffer;

typedef struct managed_buffer rt_buff_t;

/**
 * @brief Create a new managed string from a buffer of bytes.
 * @param value Buffer to initialize the managed string with.
 * @param val_len Length of the buffer `value`.
 * @return A pointer to an initialized rt_buff_t instance.
 */
extern rt_buff_t *rt_buff_new(const char *value, size_t val_len);
/**
 * @brief Get a reference to the underlying buffer of a managed string to use
 * with functions that need a raw const char pointer.
 * @attention Never pass this pointer to a function that modifies the buffer
 * in place, as this cause for UNDEFINED BEHAVIOUR, since the properties of the
 * buffer are tracked by the internal state.
 * @param self Reference to the managed string.
 * @return A reference to the underlying buffer.
 */
extern const char *rt_buff_borrow(rt_buff_t *self);
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
extern char *rt_buff_borrow_writable(rt_buff_t *self);
/**
 * @brief Returns a borrowed reference to the underlying buffer and decreases
 * the reference counter.
 * @param self Reference to the managed string.
 */
extern void rt_buff_return(rt_buff_t *self);

/**
 * @brief Destroys a managed string if it's reference counter is 0, and there
 * is no lock on it, do not use a reference to a string passed to this function
 * after calling it.
 * @param self The string to attempt destruction on.
 * @return 0 if the string was deallocated successfully
 * or the refcount otherwise.
 */
extern uint64_t rt_buff_free(rt_buff_t *self);

#endif /* END _RTHOST_STRING_ */