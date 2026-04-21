#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define RINGBUFFER_CAPACITY 256u

typedef struct {
    int16_t buffer[RINGBUFFER_CAPACITY];
    size_t mask;
    _Atomic size_t head;
    _Atomic size_t tail;
} ringbuffer_t;

void rb_init(ringbuffer_t *rb);
void rb_reset(ringbuffer_t *rb);
bool rb_push(ringbuffer_t *rb, int16_t value);
bool rb_pop(ringbuffer_t *rb, int16_t *value);
size_t rb_count(const ringbuffer_t *rb);

#endif // RINGBUFFER_H
