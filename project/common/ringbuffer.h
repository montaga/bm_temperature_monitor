#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define RINGBUFFER_CAPACITY 1024

#ifdef __cplusplus
// For C++ compatibility, use volatile instead of _Atomic
// Note: This is not thread-safe in C++, but sufficient for testing
typedef volatile size_t atomic_size_t;
extern "C" {
#else
typedef _Atomic size_t atomic_size_t;
#endif

typedef struct {
    int16_t buffer[RINGBUFFER_CAPACITY];
    size_t mask;
    atomic_size_t head;
    atomic_size_t tail;
} ringbuffer_t;

void rb_init(ringbuffer_t *rb);
void rb_reset(ringbuffer_t *rb);
bool rb_push(ringbuffer_t *rb, int16_t value);
bool rb_pop(ringbuffer_t *rb, int16_t *value);
size_t rb_count(const ringbuffer_t *rb);

#ifdef __cplusplus
}
#endif
#endif // RINGBUFFER_H
