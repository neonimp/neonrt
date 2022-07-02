#pragma once

#ifndef _NEON_RINGBUFFER_
#define _NEON_RINGBUFFER_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>

struct ring_buffer {
  uint8_t *buffer;
  size_t capacity;
  size_t size;
  size_t head;
  size_t tail;
};

typedef struct ring_buffer ring_buffer_t;

/**
 * @brief Initialize a new ring buffer, you can set all the parameters
 * bellow to create an empty buffer.
 * @param capacity The capacity of the buffer.
 * @return A new ring buffer, possibly initialized with a capacity.
 */
extern ring_buffer_t *rb_new(size_t capacity);

/**
 * @brief Free the memory allocated for the ring buffer.
 * @param self The ring buffer to free.
 */
extern void rb_free(ring_buffer_t *self);

/**
 * @brief Write data to the ring buffer.
 * @param self The ring buffer to write to.
 * @param data The data to write.
 * @param len The length of the data to write.
 * @return True if the data was written, false if the data is too large to fit
 */
extern bool rb_write(ring_buffer_t *self, const uint8_t *data, size_t len);

/**
 * @brief Read data from the ring buffer.
 * @param self The ring buffer to read from.
 * @param data The buffer to read into.
 * @param len The length of the buffer to read into.
 * @return The number of bytes read.
 */
extern size_t rb_read(ring_buffer_t *self, uint8_t *data, size_t len);

#endif // _NEON_RINGBUFFER_
