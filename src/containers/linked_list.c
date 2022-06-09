#include "linked_list.h"
#include <jemalloc/jemalloc.h>

ll_node_t *new_ll_node(void *val)
{
	ll_node_t *new_node;

	if ((new_node = malloc(sizeof(ll_node_t))) == NULL)
		return NULL;

	/* Initialize the node struct members */
	new_node->next = NULL;
	new_node->dt_ptr = val;

	return new_node;
}

dll_node_t *new_dll_node(void *val)
{
	dll_node_t *new_node;

	if ((new_node = malloc(sizeof(ll_node_t))) == NULL)
		return NULL;

	/* Initialize the node struct members */
	new_node->prev = NULL;
	new_node->next = NULL;
	new_node->dt_ptr = val;

	return new_node;
}

int ll_insert_at(ll_t *self, ll_node_t *new_node, size_t pos)
{
	ll_node_t *iter_curr;
	ll_node_t *iter_prev;
	size_t i;

	// Check for iteration bounds
	if (self->len < pos)
		return -1;

	iter_curr = self->first;

	// Iterate until we are at the requested position
	for (i = 0; i < pos; i++) {
		iter_prev = iter_curr;
		iter_curr = iter_curr->next;
	}

	iter_prev->next = new_node;
	// If there was an node here we place it as the next one, otherwise this is the tail
	new_node->next = iter_curr != NULL ? iter_curr : NULL;
	return 0;
}

ll_t *ll_new(void *fval)
{
	ll_t *new_ll;
	ll_node_t *first_node;

	if (fval != NULL) {
		if ((first_node = new_ll_node(fval)) != NULL)
			/* Failed to create the node struct */
			return NULL;
	} else {
		first_node = NULL;
	}

	if ((new_ll = malloc(sizeof(ll_t))) == NULL) {
		free(first_node);
		return NULL;
	}

	if (first_node != NULL) {
		new_ll->first = first_node;
		new_ll->last = first_node;
		new_ll->len = 1;
	} else {
		new_ll->first = NULL;
		new_ll->last = NULL;
		new_ll->len = 0;
	}

	return new_ll;
}

int ll_add_node(ll_t *self, void *val, enum LL_ADD_OP op, size_t pos)
{
	ll_node_t *new_node;

	if ((new_node = new_ll_node(val)) == NULL)
		return -1;

	switch (op) {
	case PREPEND:
		// Change the new node next to point at the previous head, and set it as first
		new_node->next = self->first;
		self->first = new_node;
		// Finally increase len
		self->len += 1;
		return 0;
	case APPEND: self->last->next = new_node;
		self->last = new_node;
		self->len += 1;
		return 0;
	case INSERT: return ll_insert_at(self, new_node, pos);
	default: return -1;
	}
}
