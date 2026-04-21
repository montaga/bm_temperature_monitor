#include "common/ringbuffer.h"
#include <stdatomic.h>

void rb_init(ringbuffer_t *rb)
{
    rb->mask = RINGBUFFER_CAPACITY - 1;
    atomic_init(&rb->head, 0);
    atomic_init(&rb->tail, 0);
}

void rb_reset(ringbuffer_t *rb)
{
    atomic_store_explicit(&rb->head, 0, memory_order_relaxed);
    atomic_store_explicit(&rb->tail, 0, memory_order_relaxed);
}

bool rb_push(ringbuffer_t *rb, int16_t value)
{
    const size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    const size_t next_head = (head + 1) & rb->mask;
    const size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if (next_head == tail) {
        const size_t next_tail = (tail + 1) & rb->mask;
        atomic_store_explicit(&rb->tail, next_tail, memory_order_release);
    }

    rb->buffer[head] = value;
    atomic_store_explicit(&rb->head, next_head, memory_order_release);
    return true;
}

bool rb_pop(ringbuffer_t *rb, int16_t *value)
{
    const size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    const size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    if (tail == head) {
        return false;
    }

    *value = rb->buffer[tail];
    atomic_store_explicit(&rb->tail, (tail + 1) & rb->mask, memory_order_release);
    return true;
}

size_t rb_count(const ringbuffer_t *rb)
{
    const size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    const size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return (head - tail) & rb->mask;
}
