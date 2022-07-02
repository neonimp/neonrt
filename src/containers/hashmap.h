/**
 * @brief Ready to use hashmaps, the hashmaps provided in the runtime have sane
 * defaults aiming for good performance under most cases, but can be tuned if
 * need be.
 * @authors Matheus Xavier <mxavier[AT]neonimp.com>
 */

#pragma once

#ifndef NEON_SRC_CONTAINERS_HASHMAP_H
#define NEON_SRC_CONTAINERS_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../neon_common.h"
#include "buffer.h"

struct hmap;
typedef struct hmap hmap_t;

struct hmap_bucket;
typedef struct hmap_bucket hmap_bucket_t;

// Defaults
#define HASHMAP_DEFAULT_CAPACITY 16
#define HASHMAP_DEFAULT_HEALTHY_THRESHOLD 75
#define HASHMAP_DEFAULT_SEED 0x19E3779B97F4A7C1
#define HASHMAP_MULTIPLIER 0x9E3779B97F4A7C13
#ifndef SIZE_MAX
#define HMAP_UNHEALTHY_THRESHOLD_DISABLE ~0UL
#else
#define HMAP_UNHEALTHY_THRESHOLD_DISABLE SIZE_MAX
#endif


// Error codes
#define HMAP_ERROR_CODE_OK 0
#define HMAP_ERROR_CODE_MALLOC_FAILED 0x00000001
#define HMAP_ERROR_CODE_KEY_MISMATCH 0x00000002
#define HMAP_ERROR_CODE_KEY_NOT_FOUND 0x00000003
#define HMAP_ERROR_CODE_KEY_ERROR 0x00000004
#define HMAP_ERROR_CODE_REHASH_FAILED 0x00000005
#define HMAP_ERROR_CODE_LOCKED 0x00000E10
#define HMAP_ERROR_CODE_UNRECOVERABLE 0xDEADBEEF

// Callback signatures
typedef uint64_t(*hmap_hash_fn_t)(const void *data, size_t data_len, uint64_t seed);
typedef void(*hmap_error_callback_t)(const hmap_t *hmap, const char *msg, uint32_t error_code);
typedef void(*hmap_unhealthy_callback_t)(hmap_t *hmap, size_t current_load);

/**
 * @brief Hashmap bucket structure.
 */
struct hmap_bucket {
  /// @brief the key of the bucket.
  neon_buff_t *key;
  /// @brief The value associated with the key.
  void *value;
  size_t value_len;
  /// @brief Whether the bucket is linked or not.
  bool is_complex;
};

/**
 * @brief Hashmap structure.
 */
struct hmap {
  /// @brief The capacity of the hashmap.
  size_t capacity;
  /// @brief Number of elements in the hashmap.
  size_t set;
  /// @brief Load factor of the hashmap.
  size_t load_factor;
  /// @brief How much of a load factor is allowed before the hashmap is resized.
  /// @note This is a percentage, so a value of 75 means that 75% of the capacity.
  /// @note Set to size_t_max to disable.
  size_t unhealthy_threshold;
  /// @brief The seed used to help spread hashes.
  size_t seed;
  /// @brief The number of collisions in the hashmap.
  size_t collisions;
  /// @brief Hash function to use.
  hmap_hash_fn_t hash_fn;
  /// @brief The buckets of the hashmap.
  hmap_bucket_t *buckets;
  /// @brief Error callback function.
  hmap_error_callback_t error_callback;
  /// @brief Unhealthy callback function.
  hmap_unhealthy_callback_t unhealthy_callback;
  char last_error_msg[256];
  /// @brief Last error code.
  uint32_t last_error_code;
  /// @brief If true, the hashmap is in an unrecoverable error state.
  bool unrecoverable_error;
  /// @brief If true, the hashmap is locked and will fail operations.
  bool locked;
};

/**
 * @brief Initialize a new hashmap.
 * @param capacity Hashmap capacity in buckets.
 * @param healthy_threshould Automatically resized hashmap capacity when load factor is reached.
 * @param seed Help avoid hashmap collisions.
 * @param hash_fn The function used to hash data.
 * @return An initialized hashmap.
 */
extern hmap_t *hmap_new(size_t capacity, size_t healthy_threshold, size_t seed, hmap_hash_fn_t hash_fn);

/**
 * @brief Initialize a new hashmap with default values.
 * @return An initialized hashmap.
 */
extern hmap_t *hmap_new_default(void);

/**
 * @brief Free a hashmap.
 * @param hmap The hashmap to free.
 */
extern void hmap_free(hmap_t *hmap);

/**
 * @brief Set a hashmap error callback.
 * @param hmap The hashmap.
 * @param error_callback The callback to set.
 */
extern void hmap_set_error_callback(hmap_t *hmap, hmap_error_callback_t error_callback);

/**
 * @brief Set a hashmap unhealthy callback, it will be called when current_load
 * is greater than healthy_threshold.
 * @param hmap The hashmap.
 * @param unhealthy_callback The callback to set.
 */
extern void hmap_set_unhealthy_callback(hmap_t *hmap, hmap_unhealthy_callback_t unhealthy_callback);

/**
 * @brief Set a value in the hashmap.
 * @param hmap The hashmap.
 * @param key The key to set.
 * @param key_len The key length.
 * @param value The value to set.
 * @param allow_update If true, the value will be updated if the key already exists.
 * @return True if the value was set, false otherwise, see hmap_last_error for more information.
 */
extern bool hmap_set(hmap_t *hmap, const void *key, size_t key_len, void *value, bool allow_update);

/**
 * @brief Get a value from the hashmap.
 * @param hmap The hashmap.
 * @param key The key to get.
 * @param key_len The key length.
 * @param value The value to get.
 * @return The value if found, NULL otherwise, see hmap_last_error for more information.
 */
extern void *hmap_get(hmap_t *hmap, const void *key, size_t key_len);

/**
 * @brief Remove a value from the hashmap.
 * @param hmap The hashmap.
 * @param key The key to remove.
 * @param key_len The key length.
 * @return The value if found, NULL otherwise, see hmap_last_error for more information.
 */
extern void *hashmap_unset(hmap_t *hmap, const void *key, size_t key_len);

/**
 * @brief Resize the hashmap.
 * @param hmap The hashmap.
 * @param new_cap The new capacity.
 */
void hmap_resize(hmap_t *hmap, size_t new_cap);

#ifdef __cplusplus
}
#endif

#endif // NEON_SRC_CONTAINERS_HASHMAP_H
