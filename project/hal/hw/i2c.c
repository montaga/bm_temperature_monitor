/* HARDWARE IMPLEMENTATION - Placeholder */

#include "hal/i2c.h"

bool i2c_read(uint8_t device_addr, uint8_t reg,
              uint8_t *data, size_t len)
{
    (void)device_addr;
    (void)reg;
    (void)data;
    (void)len;
    /* TODO: Implement I2C read for hardware */
    return false;
}

bool i2c_write(uint8_t device_addr, uint8_t reg,
               const uint8_t *data, size_t len)
{
    (void)device_addr;
    (void)reg;
    (void)data;
    (void)len;
    /* TODO: Implement I2C write for hardware */
    return false;
}
