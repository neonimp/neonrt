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

struct managed_string;

typedef struct managed_string managed_str_t;

/**
 * @brief Create a new managed string from a buffer of bytes
 * @param value Buffer to initialize the managed string with
 * @param val_len Length of the buffer `value`
 * @return An initialized hosted_str instance
 */
extern managed_str_t *managed_str_new(const char *__restrict__ value, size_t val_len);
/**
 * @brief Get a reference to the underlying buffer of a managed string to use
 * with functions that need a raw const char pointer.
 * @attention Never pass this pointer to a function that modifies the buffer
 * in place, as this cause for UNDEFINED BEHAVIOUR, since the properties of the
 * buffer are tracked by the internal state.
 * @param self Reference to the managed string
 * @return
 */
extern const char *managed_string_borrow(managed_str_t *self);
/**
 * @brief Returns a borrowed reference to the underlying buffer.
 * @param self Reference to the managed string
 */
extern void managed_string_return(managed_str_t *self);

#endif /* END _RTHOST_STRING_ */
