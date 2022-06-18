#include "linked_list.h"
#include <stdlib.h>

#define ll_check_init(self) ((self->len) > 0 && (self->first) != NULL && (self->last) != NULL)

ll_node_t *new_ll_node(void *val)
{
	ll_node_t *new_node;

	if ((new_node = malloc(sizeof(ll_node_t))) == NULL)
		return NULL;

	/* Initialize the node struct members */
	new_node->next = NULL;
	new_node->data = val;

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

ll_node_t *ll_gnat_impl(ll_node_t *first, size_t at)
{
	size_t current;
	ll_node_t *curr_n;

	if (first == NULL)
		return NULL;

	current = 0;
	curr_n = first;
	while (current < at) {
		if (curr_n->next == NULL)
			return NULL;

		curr_n = curr_n->next;
		current++;
	}

	return curr_n;
}

ll_t *ll_new(void *fval)
{
	ll_t *new_ll;
	ll_node_t *first_node;

	if (fval != NULL) {
		if ((first_node = new_ll_node(fval)) == NULL)
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

size_t ll_add_node(ll_t *self, void *val, enum LL_ADD_OP op, size_t pos)
{
	ll_node_t *new_node;
	ll_node_t *prev_n;

	if ((new_node = new_ll_node(val)) == NULL)
		return -1;

	// Check if the list is initialized if not just initialize and stop here
	if (!ll_check_init(self)) {
		self->first = new_node;
		self->last = new_node;
		self->len = 1;
	}

	switch (op) {
	case LL_PREPEND:
		// Change the new node next to point at the previous head, and set it as first
		new_node->next = self->first;
		self->first = new_node;
		// Finally increase len
		self->len += 1;
		return self->len;
	case LL_APPEND:
		// Add the new node to the tail
		self->last->next = new_node;
		self->last = new_node;
		self->len += 1;
		return self->len;
	case LL_INSERT:
		if (self->len < pos) {
			free(new_node);
			return -2;
		} else if ((prev_n = ll_gnat_impl(self->first, pos - 1)) != NULL) {
			new_node->next = prev_n->next != NULL ? prev_n->next : NULL;
			prev_n->next = new_node;
			self->len += 1;
			return self->len;
		} else {
			free(new_node);
			return -3;
		}
	default: free(new_node);
		return -1;
	}
}

void ll_remove_node(ll_t *self, size_t at)
{
	ll_node_t *ll_node;
	ll_node_t *ll_node_prev;
	size_t idx;

	idx = 0;
	ll_node_prev = NULL;
	// Check if the node exists
	if (self->len < at)
		return;

	ll_node = self->first;

	while (ll_node != NULL && idx < at - 1) {
		ll_node_prev = ll_node;
		ll_node = ll_node->next;
		idx += 1;
	}

	if (ll_node_prev != NULL && ll_node != NULL) {
		ll_node_prev->next = ll_node->next;
		self->len -= 1;
		free(ll_node);
		return;
	}
}

ll_node_t *ll_gnat(ll_t *self, size_t at)
{
	if (self->first != NULL && at < self->len)
		return ll_gnat_impl(self->first, at);
	else if (at == self->len)
		return self->last;
	else if (at == 0)
		return self->first;
	else
		return NULL;
}

void ll_free(ll_t *self)
{
	ll_node_t *iter_curr;
	ll_node_t *iter_next;

	iter_curr = self->first;

	while (iter_curr != NULL) {
		iter_next = iter_curr->next;
		free(iter_curr);
		iter_curr = iter_next;
	}

	free(self);
}
