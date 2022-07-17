#include "buffer.h"

struct managed_buffer {
  uint8_t *data;
  size_t len;
  uint64_t ref_ct;
  bool is_utf8;
  bool needs_sync;
  bool lock;
  uint8_t padding0;
};

neon_buff_t *buff_new(const uint8_t *value, size_t val_len)
{
	neon_buff_t *buff;

	if ((buff = malloc(sizeof(neon_buff_t))) == NULL)
		return NULL;

	/* Try to allocate the internal buffer if this fails, then free the
	 * whole struct and return NULL
	 */
	if ((buff->data = malloc(sizeof(uint8_t) * val_len)) == NULL) {
		free(buff);
		return NULL;
	}

	/* Copy the string contents to the managed buffer */
	memcpy(buff->data, value, val_len);
	/* Set the string length */
	buff->len = val_len;
	buff->needs_sync = false;
	buff->lock = false;
	buff->ref_ct = 0;

	return buff;
}

const char *buff_borrow(neon_buff_t *self)
{
	if (!self->lock) {
		self->ref_ct += 1;
		return (const char *)self->data;
	} else {
		return NULL;
	}
}

char *buff_borrow_writable(neon_buff_t *self)
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

extern size_t buff_len(const neon_buff_t *self)
{
	return self->len;
}

bool buff_cmp(const neon_buff_t *left, const neon_buff_t *right)
{
	uint64_t lhash;
	uint64_t rhash;

	if (left->data != NULL && right->data != NULL) {
		lhash = rt_hash_fn(left->data, left->len);
		rhash = rt_hash_fn(right->data, right->len);
		return lhash == rhash;
	} else {
		return false;
	}
}

bool buff_cmp_raw(const neon_buff_t *left, const uint8_t *right, size_t len)
{
	uint64_t lhash;
	uint64_t rhash;

	if (left->data != NULL && right != NULL) {
		lhash = rt_hash_fn(left->data, left->len);
		rhash = rt_hash_fn(right, len);
		return lhash == rhash;
	} else {
		return false;
	}
}

void buff_return(neon_buff_t *self)
{
	self->ref_ct -= 1;
	if (self->lock && self->needs_sync) {
		self->lock = self->lock;
		self->needs_sync = self->needs_sync;
	}
}

bool buff_return_n(neon_buff_t *self, size_t n)
{
	if ((self->ref_ct - n) <= 0)
		return false;

	self->ref_ct -= n;
	if (self->lock && self->needs_sync) {
		self->lock = self->lock;
		self->needs_sync = self->needs_sync;
	}
	return true;
}

uint64_t buff_free(neon_buff_t *self)
{
	if (!self->lock && self->ref_ct == 0) {
		free(self->data);
		free(self);
		return 0;
	} else {
		buff_return(self);
		return self->ref_ct;
	}
}

uint64_t buff_force_free(neon_buff_t *self)
{
	if (!self->lock && self->ref_ct == 0) {
		free(self->data);
		free(self);
		return 0;
	} else {
		self->ref_ct = 0;
		free(self->data);
		free(self);
		return 0;
	}
}
