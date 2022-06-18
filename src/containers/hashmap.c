// Ignoring this for this file as this is a false positive on this code.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "hashmap.h"
#include "../util/wire.h"
#include "linked_list.h"
#include <stdlib.h>
#include <xxhash.h>
#include <memory.h>

struct hashmap_node {
  void *data;
  rt_buff_t *key;
};

typedef struct hashmap_ll_node hmap_lln_t;

#define check_init(self) ((self) != NULL || (self)->slots != NULL)

void update_load_factor(hmap_t *self)
{
	if (self == NULL)
		return;

	// Make sure we are not dividing by 0
	if (self->set == 0 || self->len == 0) {
		self->current_load = 0;
		return;
	}
	// Calculate the load to percents in 0-100
	self->current_load = (self->set * 100) / self->len;
}

size_t calc_idx(size_t mod, uint64_t nonce, rt_buff_t *key)
{
	uint64_t hash;

	hash = XXH3_64bits_withSeed(
		rt_buff_borrow(key),
		rt_buff_sizeof(key),
		nonce);

	return hash % mod;
}

hmap_node_t *new_node(rt_buff_t *key, void *value)
{
	hmap_node_t *hmap_node;

	if ((hmap_node = malloc(sizeof(hmap_node_t))) == NULL)
		return NULL;

	hmap_node->key = key;
	hmap_node->data = value;

	return hmap_node;
}

int32_t set_key(hmap_node_t *locs, size_t len, uint64_t nonce, bool deny_collisions, rt_buff_t *key, void *value)
{
	size_t idx;
	hmap_node_t *hmap_node;
	void *tmp;

	idx = calc_idx(len, nonce, key);

	if (locs[idx].key == NULL && locs[idx].data == NULL) {
		locs[idx].key = key;
		locs[idx].data = value;
		return 0;
		// If the hashmap is allowed to resolve collisions we make this node a linked list
	} else if (!deny_collisions) {
		if ((hmap_node = new_node(locs[idx].key, locs[idx].data)) == NULL)
			return -3;

		tmp = locs[idx].data;
		// Make the key NULL to indicate this is a linked list slot
		locs[idx].key = NULL;
		// Make the node into a linked list
		locs[idx].data = ll_new(tmp);
		// Append the hmap node to the linked list
		ll_append_node(locs[idx].data, hmap_node);

		return 0;
	} else {
		// If the hashmap is set to not resolve collisions error out.
		return -1;
	}
}

void reseat_keys(hmap_t *self, hmap_node_t *new_loc, size_t new_len)
{
	size_t new_idx;
	uint8_t new_nonce[8];
	uint64_t new_nonce_int;


	new_nonce = XXH3_64bits(new_nonce, 8);
	NRT_WRITE64BE(new_nonce, (self->nonce ^ (self->current_load & self->len)));

	for (size_t idx = 0; idx < self->len; ++idx) {
		if (self->slots[idx].)
	}


	self->nonce = new_nonce_int;
}

size_t resize_map(hmap_t *self)
{
	hmap_node_t *hmap_nodes;
	size_t new_len;

	new_len = self->len * self->expand_factor;

	if ((hmap_nodes = malloc(sizeof(hmap_node_t) * new_len)) == NULL)
		return 1;

	reseat_keys(self, hmap_nodes, new_len);
	// Free the old data and replace the reference
	free(self->slots);
	self->slots = hmap_nodes;

	return 0;
}

hmap_t *hashmap_new(size_t init_len, const uint8_t *nonce, uint8_t load_lim, uint8_t expand_factor)
{
	hmap_t *new_hmap;

	if ((new_hmap = malloc(sizeof(hmap_t))) == NULL)
		return NULL;

	if ((new_hmap->slots = malloc(sizeof(hmap_node_t) * init_len)) == NULL) {
		free(new_hmap);
		return NULL;
	}

	// Set our nodes to sane defaults, don't rely on allocation being clean
	for (size_t i = 0; i < init_len; ++i) {
		new_hmap->slots[i].data = NULL;
		new_hmap->slots[i].key = NULL;
	}

	new_hmap->len = init_len;
	new_hmap->set = 0;
	new_hmap->current_load = 0;
	new_hmap->expand_factor = expand_factor;
	new_hmap->expand_trig = load_lim ? load_lim : 75;
	new_hmap->has_resolved_collisions = false;
	new_hmap->no_expand_auto = false;
	new_hmap->deny_collision = false;
	new_hmap->nonce = XXH3_64bits(nonce, 64);

	return new_hmap;
}

int32_t hashmap_set(hmap_t *self, rt_buff_t *key, void *value)
{
	int32_t result;
	// Borrow to make sure the buffer is not freed before the hashmap
	// no longer holds a reference to it
	rt_buff_borrow(key);
	// Check if the index is free to use
	if ((result = set_key(self->slots, self->len, self->nonce, self->deny_collision, key, value)) < 0) {
		return result;
	}

	update_load_factor(self);
	if (!self->no_expand_auto && self->current_load >= self->expand_trig && resize_map(self) != 0)
		return -2;

	return 0;
}

void *hashmap_get(hmap_t *self, rt_buff_t *key)
{
	size_t index;
	ll_node_t *ll_node;
	hmap_node_t *hmap_node;

	// Sanity check
	if (!check_init(self))
		return NULL;

	index = calc_idx(self->len, self->nonce, key);

	if (self->slots[index].data != NULL && self->slots[index].key != NULL) {
		// Simple slot return value directly
		return self->slots[index].data;
	} else if (self->slots[index].key == NULL && self->slots[index].data != NULL) {
		// This slot is a collision resolve, so we iterate the linked list
		ll_node = ll_gnat(self->slots[index].data, 0);
		while (ll_node != NULL) {
			hmap_node = ll_node->data;
			if (rt_buff_cmp(hmap_node->key, key))
				return hmap_node->data;

			ll_node = ll_node->next;
		}
		// If we scan the whole linked list and come up empty return NULL
		return NULL;
	} else {
		return NULL;
	}
}

void *evict_at_idx(hmap_t *self, size_t idx)
{
	void *old_val;
	old_val = self->slots[idx].data;
	self->slots[idx].key = NULL;
	self->slots[idx].data = NULL;
	self->current_load -= 1;
	update_load_factor(self);
	return old_val;
}

void *evict_at_idx_with_key(hmap_t *self, size_t idx, rt_buff_t *key)
{
	void *old_val;
	ll_node_t *ll_node;
	hmap_node_t *sub_node;
	size_t ll_idx;

	ll_idx = 0;
	sub_node = NULL;
	old_val = NULL;

	ll_node = ll_gnat(self->slots[idx].data, 0);

	while (ll_node != NULL) {
		sub_node = ll_node->data;
		if (rt_buff_cmp(sub_node->key, key)) {
			old_val = sub_node->data;
			ll_remove_node(self->slots[idx].data, ll_idx);
			break;
		}
		ll_idx += 1;
		ll_node = ll_node->next;
	}

	self->current_load -= 1;
	update_load_factor(self);
	return old_val;
}

void *hashmap_evict(hmap_t *self, rt_buff_t *key)
{
	size_t idx;

	// Sanity check
	if (!check_init(self))
		return NULL;

	idx = calc_idx(self->len, self->nonce, key);
	if (self->slots[idx].key != NULL && self->slots[idx].data != NULL)
		return evict_at_idx(self, idx);
	else if (self->slots[idx].key == NULL && self->slots[idx].data != NULL)
		return evict_at_idx_with_key(self, idx, key);
	else
		return NULL;
}

void hashmap_evict_all(hmap_t *self)
{
	// Sanity check
	if (!check_init(self))
		return;

	for (size_t i = 0; i < self->len; i++) {
		if (self->slots[i].data != NULL)
			hashmap_evict(self, self->slots[i].key);
	}
	self->current_load = 0;
	update_load_factor(self);
}

void hashmap_free(hmap_t *self)
{
	ll_node_t *cnode;

	// Sanity check
	if (!check_init(self))
		return;

	for (size_t idx = 0; idx < self->len; idx++) {
		if (self->slots[idx].key != NULL) {
			rt_buff_return(self->slots[idx].key);
			rt_buff_free(self->slots[idx].key);
		}
		if (self->slots[idx].key == NULL && self->slots[idx].data != NULL) {
			cnode = ll_gnat(self->slots[idx].data, 0);
			while (cnode != NULL) {
				free(cnode->data);
				cnode = cnode->next;
			}
			ll_free(self->slots[idx].data);
		}
	}

	free(self->slots);
	free(self);
}

#pragma clang diagnostic pop