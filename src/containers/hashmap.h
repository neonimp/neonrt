/**
 * @brief Ready to use hashmaps
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef _RTHOST_HASHMAP_
#define _RTHOST_HASHMAP_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../rt_buffer/rt_buffer.h"

/**
 * @brief A single node of the hashmap
 */
struct hashmap_node;

typedef struct hashmap_node hmap_node_t;

/**
 * @brief The container of a hashmap
 * @attention One should never write to any of these fields directly, if you do
 * make sure to check hashmap.c for implementation details.
 */
struct hashmap_container {
  hmap_node_t *data;
  size_t len;
  size_t set;
  /**
   * A nonce used when hashing to make collisions more difficult
   * @attention The nonce of a hashmap should never be changed once set or
   * keys will become irretrievable by hash
   */
  uint64_t nonce;
  /// How much load the hashmap is under currently
  uint32_t current_load;
  /// How much load can trigger an expansion
  uint32_t expand_trig;
  /// Expand by this factor when expansion is triggered
  uint8_t expand_factor;
  /// Disable expansion autonomy, should be left alone in most cases
  uint8_t no_expand_auto: 1;
  uint8_t reserved: 7;
};

typedef struct hashmap_container hmap_t;

/**
 * @brief Initialize a new hashmap with the options defined here, check the
 * struct hashmap_container for details on what each one does.
 * @param init_len How many nodes worth of space to pre-allocate
 * @param nonce Append 64 bytes of this value to keys before hashing, if null
 * nothing is appended.
 * @param load_lim Maximum load factor (used slots/currently allocated) before
 * triggering expansion.
 * @param expand_factor When expansion is triggered expand by this factor,
 * recommended is 2.
 * @return A newly initialized hashmap configured with the received parameters
 */
extern hmap_t *hashmap_new(size_t init_len, uint8_t *nonce, uint32_t load_lim, uint8_t expand_factor);

/**
 * @brief Set the key to value on self
 * @param self The hashmap to set the key on
 * @param key A rt_buff to use as a key
 * @param value Pointer to any value
 * @return 0 on success 1 otherwise
 */
extern int32_t hashmap_set(hmap_t *self, rt_buff_t *key, void *value);

/**
 * @brief
 * @param self The hashmap to operate on
 * @param key A rt_buff to use as a key
 * @return
 */
extern void *hashmap_get(hmap_t *self, rt_buff_t *key);

/**
 * @brief Mark a node for reuse, and return the value that was stored in it
 * @param self The hashmap to operate on
 * @param key A rt_buff to use as a key
 * @return The value held in the key previously
 */
extern void *hashmap_evict(hmap_t *self, rt_buff_t *key);

/**
 * @brief Evict all the slots in the hashmap, effectively clearing it
 * @param self The hashmap to clear
 */
extern void hashmap_evict_all(hmap_t *self);

/**
 * @brief Release the memory associated to a hashmap and it's nodes
 * @param self The hashmap to destroy
 */
extern void hashmap_free(hmap_t *self);

#endif /* END _RTHOST_HASHMAP_ */
