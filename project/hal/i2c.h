#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define EEPROM_ADDR 0x50

#define REG_HW_REV 0x00
#define REG_SERIAL 0x10


bool i2c_read(uint8_t device_addr, uint8_t reg,
              uint8_t *data, size_t len);

bool i2c_write(uint8_t device_addr, uint8_t reg,
               const uint8_t *data, size_t len);

#endif