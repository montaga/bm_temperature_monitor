#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdbool.h>
#include "app/config_loader.h"
#include "app/temp_monitor.h"
#include "common/ringbuffer.h"
#include "hal/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Application state structure
    typedef struct
    {
        config_t config;
        ringbuffer_t sensor_buffer;
        temp_monitor_t monitor;
        bool initialized;
    } app_context_t;

    // Initialize the application
    // Returns true on success, false on failure
    bool app_init(app_context_t *ctx);

    // Process one iteration of the application loop
    // This includes temperature processing, LED updates, and status printing
    void app_process(app_context_t *ctx);

    // Clean up application resources
    void app_cleanup(app_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // APPLICATION_H