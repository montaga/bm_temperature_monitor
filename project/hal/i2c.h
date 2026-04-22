#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool i2c_read(uint8_t device_addr, uint8_t reg,
              uint8_t *data, size_t len);

bool i2c_write(uint8_t device_addr, uint8_t reg,
               const uint8_t *data, size_t len);

#endif