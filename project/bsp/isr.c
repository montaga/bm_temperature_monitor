#define _XOPEN_SOURCE 600

#include "bsp/isr.h"
#include "hal/adc.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>

static ringbuffer_t *s_buffer = NULL;
static atomic_bool s_isr_running = false;
static pthread_t s_isr_thread;

#ifdef EMULATED_ISR
// isr_thread_entry will not be implemented on actual hardware
// Instead adc_isr would be called by the hardware interrupt mechanism
static void *isr_thread_entry(void *arg)
{
    (void)arg;
    while (atomic_load_explicit(&s_isr_running, memory_order_acquire))
    {
        usleep(100);
        adc_isr();
    }
    return NULL;
}
#endif

// adc_isr will be called by the hardware interrupt mechanism on actual hardware, but here with -D EMULATED_ISR we call it from a thread to simulate periodic ADC sampling
void adc_isr(void)
{
    if (s_buffer == NULL)
    {
        return;
    }

    int16_t raw = adc_read();
    rb_push(s_buffer, raw);
}

bool isr_start(ringbuffer_t *buffer)
{
    if (buffer == NULL)
    {
        return false;
    }

    s_buffer = buffer;
    atomic_store_explicit(&s_isr_running, true, memory_order_release);
#ifdef EMULATED_ISR
    return pthread_create(&s_isr_thread, NULL, isr_thread_entry, NULL) == 0;
#endif
    return true;
}

void isr_stop(void)
{
    atomic_store_explicit(&s_isr_running, false, memory_order_release);
    pthread_join(s_isr_thread, NULL);
}
