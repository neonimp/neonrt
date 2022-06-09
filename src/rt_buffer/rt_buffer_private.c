#include "rt_buffer.h"
#include "rt_buffer_private.h"
#include <jemalloc/jemalloc.h>
#include <memory.h>

rt_buff_t *rt_buff_new(const char *value, size_t val_len)
{
	rt_buff_t *new_str;

	if ((new_str = malloc(sizeof(rt_buff_t))) == NULL)
		return NULL;

	/* Try to allocate the internal buffer if this fails, then free the
	 * whole struct and return NULL
	 */
	if ((new_str->data = malloc(sizeof(uint8_t) * val_len)) == NULL) {
		free(new_str);
		return NULL;
	}

	/* Copy the string contents to the managed buffer */
	memcpy(new_str->data, value, val_len);
	/* Set the string length */
	new_str->strlen = val_len;
	new_str->needs_sync = false;
	new_str->lock = false;
	new_str->ref_ct = 0;

	return new_str;
}

const char *managed_string_borrow(rt_buff_t *self)
{
	if (!self->lock) {
		self->ref_ct += 1;
		return (const char *)self->data;
	} else {
		return NULL;
	}
}

char *managed_string_borrow_writable(rt_buff_t *self)
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


