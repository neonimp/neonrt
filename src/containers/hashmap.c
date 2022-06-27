#include "hashmap.h"
#include "../util/wire.h"
#include "linked_list.h"
#include <xxhash.h>
#include <memory.h>

struct hmap_linked_node {
  rt_buff_t *key;
  void *value;
};

typedef struct hmap_linked_node hmap_linked_node_t;

void default_error_callback(const hmap_t *hmap, const char *msg, uint32_t error_code)
{
	(void)hmap;
	(void)msg;
	(void)error_code;
}

void update_load_factor(hmap_t *hmap)
{
	hmap->load_factor = (hmap->set * 100) / hmap->capacity;
}

bool hmap_check(hmap_t *hmap, const char *fn_name)
{
	if (hmap->locked) {
		sprintf(hmap->last_error_msg, "%s: hashmap is locked", fn_name);
		hmap->last_error_code = HMAP_ERROR_CODE_LOCKED;
		hmap->error_callback(hmap, hmap->last_error_msg, hmap->last_error_code);
		return false;
	} else if (hmap->unrecoverable_error) {
		sprintf(hmap->last_error_msg, "%s: hashmap is in an unrecoverable error state", fn_name);
		hmap->last_error_code = HMAP_ERROR_CODE_UNRECOVERABLE;
		hmap->error_callback(hmap, hmap->last_error_msg, hmap->last_error_code);
		return false;
	} else {
		return true;
	}
}

void hmap_set_error(hmap_t *hmap, const char *msg, const char *fn_name, uint32_t error_code)
{
	sprintf(hmap->last_error_msg, "%s: %s", fn_name, msg);
	hmap->last_error_code = error_code;
	hmap->error_callback(hmap, hmap->last_error_msg, hmap->last_error_code);
}

void *linked_list_find(linked_list_t *list, const void *key, size_t key_len)
{
	ll_node_t *node;
	hmap_linked_node_t *node_value;

	node = list->first;

	while (node != NULL) {
		node_value = (hmap_linked_node_t *)node->data;
		if (rt_buff_cmp_raw(node_value->key, (const uint8_t *)key, key_len)) {
			return node_value->value;
		}
		node = node->next;
	}

	return NULL;
}

void *hmap_complex_key_get_value(const hmap_bucket_t *bucket, const void *key, size_t key_len)
{
	linked_list_t *list;
	void *value;

	if (bucket->is_complex) {
		list = (linked_list_t *)bucket->value;
		if ((value = linked_list_find(list, key, key_len)) != NULL) {
			return value;
		}
	}
	return NULL;
}

void hmap_complex_key_set_value(hmap_t *hmap, hmap_bucket_t *bucket, const void *key, size_t key_len, void *value)
{
	hmap_linked_node_t *node_value;
	rt_buff_t *key_buff;

	key_buff = rt_buff_new_from_raw(key, key_len);

	if (!bucket->is_complex) {
		node_value = (hmap_linked_node_t *)malloc(sizeof(hmap_linked_node_t));
		if (!node_value) {
			hmap_set_error(hmap, "malloc failed", __FUNCTION__, HMAP_ERROR_CODE_MALLOC_FAILED);
			return;
		}
		node_value->key = bucket->key;
		node_value->value = bucket->value;
		bucket->value = ll_new(node_value);

		node_value = (hmap_linked_node_t *)malloc(sizeof(hmap_linked_node_t));
		if (!node_value) {
			hmap_set_error(hmap, "malloc failed", __FUNCTION__, HMAP_ERROR_CODE_MALLOC_FAILED);
			return;
		}
		node_value->key = key_buff;
		node_value->value = value;
		ll_append_node(bucket->value, node_value);

		bucket->key = NULL;
		bucket->is_complex = true;
		bucket->value_len += 2;
	} else {
		node_value = (hmap_linked_node_t *)malloc(sizeof(hmap_linked_node_t));
		if (!node_value) {
			hmap_set_error(hmap, "malloc failed", __FUNCTION__, HMAP_ERROR_CODE_MALLOC_FAILED);
			return;
		}
		node_value->key = key_buff;
		node_value->value = value;
		ll_add_node(bucket->value, node_value, LL_APPEND, 0);
		bucket->value_len += 1;
	}
	hmap->collisions += 1;
}

void *hmap_complex_key_unset_value(hmap_bucket_t *bucket, const void *key, size_t key_len)
{
	void *value;
	size_t at_index;
	ll_node_t *node;
	hmap_linked_node_t *node_value;
	linked_list_t *list;

	list = (linked_list_t *)bucket->value;
	node = list->first;
	at_index = 0;

	while (node != NULL) {
		node_value = (hmap_linked_node_t *)node->data;
		if (rt_buff_cmp_raw(node_value->key, (const uint8_t *)key, key_len)) {
			value = node_value->value;
			free(node_value);
			ll_remove_node(list, at_index);
			return value;
		}
		node = node->next;
		at_index++;
	}

	return NULL;
}

size_t hmap_index(const hmap_t *hmap, const void *key, size_t key_len)
{
	size_t index = hmap->hash_fn(key, key_len, hmap->seed) % hmap->capacity;
	return index;
}

hmap_t *hmap_new(size_t capacity, size_t healthy_threshold, size_t seed, hmap_hash_fn_t hash_fn)
{
	hmap_t *hmap = hmap_malloc_fn(sizeof(hmap_t));
	if (!hmap) {
		return NULL;
	}

	hmap->capacity = capacity ? capacity : HASHMAP_DEFAULT_CAPACITY;
	hmap->set = 0;
	hmap->load_factor = 0;
	hmap->healthy_threshold = healthy_threshold ? healthy_threshold : HASHMAP_DEFAULT_HEALTHY_THRESHOLD;
	hmap->seed = seed ? seed : HASHMAP_DEFAULT_SEED;
	hmap->hash_fn = hash_fn ? hash_fn : &XXH3_64bits_withSeed;
	hmap->buckets = hmap_malloc_fn(sizeof(hmap_bucket_t) * capacity);
	hmap->error_callback = default_error_callback;
	hmap->last_error_code = 0;
	hmap->unrecoverable_error = false;
	hmap->collisions = 0;
	hmap->locked = false;
	memset(hmap->last_error_msg, 0, 256);
	memset(hmap->buckets, 0, sizeof(hmap_bucket_t) * capacity);
	return hmap;
}

hmap_t *hmap_new_default(void)
{
	return hmap_new(HASHMAP_DEFAULT_CAPACITY, HASHMAP_DEFAULT_HEALTHY_THRESHOLD, HASHMAP_DEFAULT_SEED, NULL);
}

void hmap_free(hmap_t *hmap)
{
	if (hmap) {
		for (size_t i = 0; i < hmap->capacity; i++) {
			hmap_bucket_t *bucket = &hmap->buckets[i];
			if (bucket->is_complex) {
				ll_free(bucket->value);
			}
			if (bucket->key) {
				rt_buff_free(bucket->key);
			}
		}

		hmap_free_fn(hmap->buckets);
		hmap_free_fn(hmap);
	}
}

void hmap_set_error_callback(hmap_t *hmap, hmap_error_callback_t error_callback)
{
	hmap->error_callback = error_callback;
}

void *hmap_get(hmap_t *hmap, const void *key, size_t key_len)
{
	size_t index;
	hmap_bucket_t *bucket;

	if (!hmap_check(hmap, "hmap_get"))
		return false;

	index = hmap_index(hmap, key, key_len);
	bucket = &hmap->buckets[index];
	if (bucket->is_complex) {
		return hmap_complex_key_get_value(bucket, key, key_len);
	} else if (bucket->key) {
		if (rt_buff_cmp_raw(bucket->key, key, key_len)) {
			return bucket->value;
		} else if (!bucket->is_complex) {
			hmap_set_error(hmap, "key does not match", "hmap_get", HMAP_ERROR_CODE_KEY_MISMATCH);
			return NULL;
		}
	} else {
		hmap_set_error(hmap, "key does not exist", "hmap_get", HMAP_ERROR_CODE_KEY_MISMATCH);
		return NULL;
	}
	return NULL;
}

bool hmap_set(hmap_t *hmap, const void *key, size_t key_len, void *value, bool allow_update)
{
	size_t index;
	hmap_bucket_t *bucket;
	rt_buff_t *key_buff;

	if (!hmap_check(hmap, "hmap_set"))
		return false;

	index = hmap_index(hmap, key, key_len);
	bucket = &hmap->buckets[index];

	if (bucket->is_complex) {
		hmap_complex_key_set_value(hmap, bucket, key, key_len, value);
		hmap->set++;
		update_load_factor(hmap);
		return true;
	} else if (bucket->key) {
		if (rt_buff_cmp_raw(bucket->key, key, key_len)) {
			if (allow_update) {
				bucket->value = value;
				hmap->set++;
				update_load_factor(hmap);
				return true;
			} else {
				hmap_set_error(hmap, "key already exists", "hmap_set", HMAP_ERROR_CODE_KEY_MISMATCH);
				return false;
			}
		} else {
			hmap_complex_key_set_value(hmap, bucket, key, key_len, value);
			hmap->set++;
			update_load_factor(hmap);
			return true;
		}
	} else if (!bucket->key && !bucket->is_complex) {
		key_buff = rt_buff_new_from_raw(key, key_len);
		bucket->key = key_buff;
		bucket->value = value;
		bucket->is_complex = false;
		hmap->set++;
		update_load_factor(hmap);
		return true;
	} else {
		hmap_set_error(hmap, "key does not exist", "hmap_set", HMAP_ERROR_CODE_KEY_MISMATCH);
		return false;
	}
}

void *hashmap_unset(hmap_t *hmap, const void *key, size_t key_len)
{
	size_t index;
	hmap_bucket_t *bucket;
	void *value;

	if (!hmap_check(hmap, "hashmap_unset"))
		return false;

	index = hmap_index(hmap, key, key_len);
	bucket = &hmap->buckets[index];

	if (bucket->is_complex) {
		value = hmap_complex_key_unset_value(bucket, key, key_len);
		return value;
	} else if (bucket->key) {
		value = bucket->value;
		bucket->value = NULL;
		rt_buff_free(bucket->key);
		bucket->key = NULL;
		update_load_factor(hmap);
		hmap->set--;
		return value;
	} else {
		hmap_set_error(hmap, "key does not exist", "hashmap_unset", HMAP_ERROR_CODE_KEY_MISMATCH);
		return NULL;
	}
}
