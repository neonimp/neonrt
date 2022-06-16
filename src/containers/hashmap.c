#include "hashmap.h"
#include <stdlib.h>
#include <xxhash.h>
#include <memory.h>

struct hashmap_node {
  bool reclaim;
  void *data;
  rt_buff_t *key;
};

#define check_init(self) ((self) != NULL || (self)->data != NULL)

void update_load_factor(hmap_t *self)
{
	if (self == NULL)
		return;

	if (self->set == 0)
		self->current_load = 0;

	// All preconditions met, now we can spend time to calculate
	self->current_load = self->set / self->len;
}

size_t resize_map(hmap_t *self)
{
	hmap_node_t *new_ptr;
	size_t i;

	// Allocate the expanded array
	new_ptr = malloc(sizeof(hmap_node_t) * (self->len * self->expand_factor));

	if (new_ptr == NULL)
		return -1;

	// Copy the old data to the new data array
	memcpy(new_ptr, self->data, sizeof(hmap_node_t) * self->len);

	// Set the iteration index to the current length to avoid
	// overwriting data.
	i = self->len;

	// Update the length
	self->len = self->len * self->expand_factor;

	// Initialize all new slots to neutral values
	for (; i < self->len; ++i) {
		new_ptr[i].reclaim = true;
		new_ptr[i].data = NULL;
		new_ptr[i].key = NULL;
	}

	// Free the old array
	free(self->data);

	// Update the data pointer to the new aray
	self->data = new_ptr;
	// Update the stored load factor
	update_load_factor(self);

	return self->len;
}


size_t make_index(size_t mod, uint64_t nonce, rt_buff_t *key)
{
	uint64_t hash;

	hash = XXH3_64bits_withSeed(
		rt_buff_borrow(key),
		rt_buff_sizeof(key),
		nonce);

	return hash % mod;
}

hmap_t *hashmap_new(size_t init_len, const uint8_t *nonce, uint32_t load_lim, uint8_t expand_factor)
{
	hmap_t *new_hmap;

	if ((new_hmap = malloc(sizeof(hmap_t))) == NULL)
		return NULL;

	if ((new_hmap->data = malloc(sizeof(hmap_node_t) * init_len)) == NULL) {
		free(new_hmap);
		return NULL;
	}

	// Set our nodes to sane defaults, don't rely on allocation being clean
	for (size_t i = 0; i < init_len; ++i) {
		new_hmap->data[i].reclaim = true;
		new_hmap->data[i].data = NULL;
		new_hmap->data[i].key = NULL;
	}

	new_hmap->len = init_len;
	new_hmap->set = 0;
	new_hmap->current_load = 0;
	new_hmap->expand_factor = expand_factor;
	new_hmap->expand_trig = load_lim;
	new_hmap->no_expand_auto = false;
	new_hmap->nonce = XXH3_64bits(nonce, 64);

	return new_hmap;
}

int32_t hashmap_set(hmap_t *self, rt_buff_t *key, void *value)
{
	size_t index;

	// Sanity check
	if (!check_init(self))
		return -1;

	index = make_index(self->len, self->nonce, key);

	rt_buff_borrow(key);
	if (self->data[index].reclaim || self->data[index].key == NULL) {
		self->data[index].key = key;
		self->data[index].data = value;
		self->data[index].reclaim = false;
		self->set += 1;
		update_load_factor(self);
		if (!self->no_expand_auto && self->current_load >= self->expand_trig)
			resize_map(self);
		return 0;
	}

	return -1;
}

void *hashmap_get(hmap_t *self, rt_buff_t *key)
{
	size_t index;

	// Sanity check
	if (!check_init(self))
		return NULL;

	index = make_index(self->len, self->nonce, key);

	if (!self->data[index].reclaim && self->data[index].key != NULL)
		return self->data[index].data;
	else
		return NULL;
}

void *hashmap_evict(hmap_t *self, rt_buff_t *key)
{
	size_t index;

	// Sanity check
	if (!check_init(self))
		return NULL;

	index = make_index(self->len, self->nonce, key);

	self->data[index].reclaim = true;
	return self->data[index].data;
}

void hashmap_evict_all(hmap_t *self)
{
	// Sanity check
	if (!check_init(self))
		return;

	for (size_t i = 0; i < self->len; i++) {
		self->data[i].reclaim = true;
	}
}

void hashmap_free(hmap_t *self)
{
	// Sanity check
	if (!check_init(self))
		return;

	for (size_t i = 0; i < self->len; i++) {
		if (self->data[i].key != NULL) {
			rt_buff_return(self->data[i].key);
			rt_buff_free(self->data[i].key);
		}
	}

	free(self->data);
	free(self);
}
