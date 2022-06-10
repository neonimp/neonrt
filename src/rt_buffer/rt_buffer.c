#include "rt_buffer.h"
#include "rt_buffer_private.h"
#include <stdlib.h>
#include <memory.h>

rt_buff_t *rt_buff_new(uint8_t *value, size_t val_len)
{
	rt_buff_t *rt_buff;

	if ((rt_buff = malloc(sizeof(rt_buff_t))) == NULL)
		return NULL;

	/* Try to allocate the internal buffer if this fails, then free the
	 * whole struct and return NULL
	 */
	if ((rt_buff->data = malloc(sizeof(uint8_t) * val_len)) == NULL) {
		free(rt_buff);
		return NULL;
	}

	/* Copy the string contents to the managed buffer */
	memcpy(rt_buff->data, value, val_len);
	/* Set the string length */
	rt_buff->len = val_len;
	rt_buff->needs_sync = false;
	rt_buff->lock = false;
	rt_buff->ref_ct = 0;

	return rt_buff;
}

const char *rt_buff_borrow(rt_buff_t *self)
{
	if (!self->lock) {
		self->ref_ct += 1;
		return (const char *)self->data;
	} else {
		return NULL;
	}
}

char *rt_buff_borrow_writable(rt_buff_t *self)
{
	/* To lease out a write-able ref it needs to be the only ref */
	if (self->ref_ct == 0 && !self->lock) {
		/* Locks the string because we are leasing out a non
		 * readonly ref, and it will also need a synchronisation.
		 */
		self->lock = true;
		self->needs_sync = true;
		return (char *)self->data;
	} else {
		return NULL;
	}
}

extern size_t rt_buff_sizeof(const rt_buff_t *self)
{
	return self->len;
}

void rt_buff_return(rt_buff_t *self)
{
	self->ref_ct -= 1;
	if (self->lock && self->needs_sync) {
		self->lock = self->lock;
		self->needs_sync = self->needs_sync;
	}
}

uint64_t rt_buff_free(rt_buff_t *self)
{
	if (!self->lock && self->ref_ct == 0) {
		free(self->data);
		free(self);
		return 0;
	} else {
		rt_buff_return(self);
		return self->ref_ct;
	}
}
