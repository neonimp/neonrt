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
  bool evicted;
  bool ll_container;
  void *data;
  rt_buff_t *key;
};

struct hashmap_ll_node {
  rt_buff_t *key;
  void *data;
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

hmap_lln_t *new_hmap_ll_node(rt_buff_t *key, void *data)
{
	hmap_lln_t *node;

	if ((node = malloc(sizeof(hmap_lln_t))) == NULL)
		return NULL;

	node->key = key;
	node->data = data;
	return node;
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

void reseat_keys(hmap_t *self, size_t old_sz)
{
	ll_node_t *cnode;
	hmap_lln_t *cnode_v;
	uint8_t n_nonce[8];

	NRT_WRITE64BE(n_nonce, self->nonce);
	self->nonce = XXH3_64bits(n_nonce, 8);
	self->no_expand_auto = true;
	for (size_t i = 0; i < old_sz; ++i) {
		// Skip evicted keys
		if (self->slots[i].evicted)
			continue;

		if (!self->slots[i].ll_container) {
			self->slots[i].evicted = true;
			// Not a collision resolution, so just move
			hashmap_set(self, self->slots[i].key, self->slots[i].data);
		} else if(self->slots[i].ll_container && !self->slots[i].evicted) {
			// On a collision resolution node try to flatten
			// This slot is a collision resolve, so we iterate the linked list
			if ((cnode = ll_gnat(self->slots[i].data, 0)) == NULL)
				continue;
			self->slots[i].evicted = true;
			while (cnode != NULL) {
				cnode_v = cnode->dt_ptr;
				hashmap_set(self, cnode_v->key, cnode_v->data);
				free(cnode->dt_ptr);
				cnode = cnode->next;
			}
			ll_free(self->slots[i].data);
		}
	}
	self->no_expand_auto = false;
}

size_t resize_map(hmap_t *self)
{
	hmap_node_t *node_store;
	size_t old_sz;
	size_t i;

	// Allocate the expanded array
	node_store = malloc(sizeof(hmap_node_t) * (self->len * self->expand_factor));

	if (node_store == NULL)
		return -1;

	// Copy the old data to the new data array
	memcpy(node_store, self->slots, sizeof(hmap_node_t) * self->len);

	// Set the iteration index to the current length to avoid
	// overwriting data.
	i = self->len;
	old_sz = self->len;

	// Update the length
	self->len = self->len * self->expand_factor;

	// Initialize all new slots to neutral values
	for (; i < self->len; ++i) {
		node_store[i].evicted = true;
		node_store[i].ll_container = false;
		node_store[i].data = NULL;
		node_store[i].key = NULL;
	}

	// Free the old array
	free(self->slots);

	// Update the data pointer to the new aray
	self->slots = node_store;
	// Update the stored load factor
	// Reset the set counter because reseat calls hashmap_set
	self->set = 0;
	reseat_keys(self, old_sz);
	update_load_factor(self);
	return self->len;
}

int32_t hmap_make_ll_slot(hmap_t *self, size_t index, rt_buff_t *key, void *value)
{
	hmap_lln_t *lln;

	// Check if the node at index is not already a linked list
	if (!self->slots[index].ll_container) {
		// Create a new hmap_ll_node with the current data
		if ((lln = new_hmap_ll_node(self->slots[index].key, self->slots[index].data)) == NULL)
			return -1;

		// Create a new linked list on this slot
		self->slots[index].data = ll_new(lln);
		if (self->slots[index].data == NULL)
			return -2;

		// Make the new values into another hmap_ll_node
		if ((lln = new_hmap_ll_node(key, value)) == NULL)
			return -3;

		// Append the node to the linked list
		ll_append_node(self->slots[index].data, lln);

		// Mark this hashmap as having resolved collisions
		self->has_resolved_collisions = true;
		// Remove the old key
		self->slots[index].key = NULL;
		self->set += 1;
		update_load_factor(self);
		// If not we initialize it to one
		self->slots[index].ll_container = true;
		return 1;
	} else {
		if (self->slots[index].data == NULL)
			return -3;
		// The slot already contains a linked list, just append
		if ((lln = new_hmap_ll_node(key, value)) == NULL)
			return -4;

		// Append the node to the linked list
		ll_append_node(self->slots[index].data, lln);

		self->set += 1;
		update_load_factor(self);
		return 2;
	}
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
		new_hmap->slots[i].evicted = true;
		new_hmap->slots[i].ll_container = false;
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
	size_t index;

	// Sanity check
	if (!check_init(self))
		return -1;

	index = calc_idx(self->len, self->nonce, key);

	// Borrow to make sure the buffer is not freed before the hashmap
	// no longer holds a reference to it
	rt_buff_borrow(key);
	// Check if the index is free to use
	if (self->slots[index].evicted && self->slots[index].key == NULL) {
		self->slots[index].key = key;
		self->slots[index].data = value;
		self->slots[index].evicted = false;
		self->set += 1;
		update_load_factor(self);
		if (!self->no_expand_auto && self->current_load >= self->expand_trig && resize_map(self) == 0)
			return -1;

		return 0;
		// If the hashmap is allowed to resolve collisions we make this node a linked list
	} else if (!self->deny_collision) {
		return hmap_make_ll_slot(self, index, key, value);
	} else {
		// If the hashmap is set to not resolve collisions error out.
		return -1;
	}
}

void *hashmap_get(hmap_t *self, rt_buff_t *key)
{
	size_t index;
	ll_node_t *cnode;
	hmap_lln_t const *cnode_v;

	// Sanity check
	if (!check_init(self))
		return NULL;

	index = calc_idx(self->len, self->nonce, key);

	if (!self->slots[index].evicted && self->slots[index].key != NULL && !self->slots[index].ll_container) {
		return self->slots[index].data;
	} else if (self->slots[index].ll_container) {
		// This slot is a collision resolve, so we iterate the linked list
		cnode = ll_gnat(self->slots[index].data, 0);
		while (cnode != NULL) {
			cnode_v = cnode->dt_ptr;
			if (rt_buff_cmp(cnode_v->key, key))
				return cnode_v->data;

			cnode = cnode->next;
		}
		// If we scan the whole linked list and come up empty return NULL
		return NULL;
	} else {
		return NULL;
	}
}

void *hashmap_evict(hmap_t *self, rt_buff_t *key)
{
	size_t index;

	// Sanity check
	if (!check_init(self))
		return NULL;

	index = calc_idx(self->len, self->nonce, key);

	self->slots[index].evicted = true;
	self->current_load -= 1;
	update_load_factor(self);
	return self->slots[index].data;
}

void hashmap_evict_all(hmap_t *self)
{
	// Sanity check
	if (!check_init(self))
		return;

	for (size_t i = 0; i < self->len; i++) {
		self->slots[i].evicted = true;
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

	for (size_t i = 0; i < self->len; i++) {
		if (self->slots[i].key != NULL) {
			rt_buff_return(self->slots[i].key);
			rt_buff_free(self->slots[i].key);
		}
		if (self->slots[i].ll_container) {
			cnode = ll_gnat(self->slots[i].data, 0);
			while (cnode != NULL) {
				free(cnode->dt_ptr);
				cnode = cnode->next;
			}
			ll_free(self->slots[i].data);
		}
	}

	free(self->slots);
	free(self);
}

#pragma clang diagnostic pop