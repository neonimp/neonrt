#include "ringbuffer.h"

ring_buffer_t *rb_new(size_t capacity)
{
	ring_buffer_t *self;

	self = malloc(sizeof(ring_buffer_t));
	if (self == NULL) {
		return NULL;
	}
	self->buffer = malloc(capacity);
	if (self->buffer == NULL) {
		free(self);
		return NULL;
	}
	self->capacity = capacity;
	self->size = 0;
	self->head = 0;
	self->tail = 0;
	return self;
}

void rb_free(ring_buffer_t *self)
{
	free(self->buffer);
	free(self);
}

bool rb_write(ring_buffer_t *self, const uint8_t *data, size_t len)
{
	size_t tail;
	size_t capacity;

	tail = self->tail;
	capacity = self->capacity;

	if (len > capacity - self->size) {
		return false;
	}

	for (size_t i = 0; i < len; i++) {
		self->buffer[tail] = data[i];
		tail = (tail + 1) % capacity;
	}
	self->tail = tail;
	self->size += len;
	return true;
}

size_t rb_read(ring_buffer_t *self, uint8_t *data, size_t len)
{
	size_t head;
	size_t capacity;
	size_t read;

	head = self->head;
	capacity = self->capacity;
	read = 0;

	for (size_t i = 0; i < len; i++) {
		data[i] = self->buffer[head];
		head = (head + 1) % capacity;
		if (head == self->tail) {
			break;
		}
		read++;
	}

	self->head = head;
	self->size -= len;
	return read;
}
