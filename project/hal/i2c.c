#include "i2c.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define EEPROM_ADDR 0x50

// EEPROM Register Map (Mock)
#define REG_HW_REV 0x00
#define REG_SERIAL 0x10
#define SERIAL_LEN 16

static uint8_t eeprom_memory[256];

// NOTE:
// This mock simulates a simple EEPROM with byte-addressable memory.
// In real hardware, I2C transactions would include start/stop conditions,
// address phase, and possible write-cycle delays.

static void eeprom_mock_init(void)
{
    static int initialized = 0;
    if (initialized)
        return;

    initialized = 1;

    // hardware revision
    const char *env_rev = getenv("SIM_HW_REV");
    if (env_rev != NULL && env_rev[0] == 'A')
    {
        eeprom_memory[REG_HW_REV] = 0; // Rev-A
    }
    else
    {
        eeprom_memory[REG_HW_REV] = 1; // defailt to Rev-B
    }

    const char *serial = "ABC1234";
    strncpy((char *)&eeprom_memory[REG_SERIAL], serial, SERIAL_LEN);

    eeprom_memory[REG_SERIAL + SERIAL_LEN - 1] = '\0';
}

// Mock I2C read function simulating EEPROM read behavior
bool i2c_read(uint8_t device_addr, uint8_t reg,
              uint8_t *data, size_t len)
{
    eeprom_mock_init();

    if (data == NULL || len == 0)
        return false;

    if (device_addr != EEPROM_ADDR)
    {
        printf("[I2C] ERROR: Unknown device address 0x%02X\n", device_addr);
        return false;
    }

    if ((reg + len) > sizeof(eeprom_memory))
    {
        printf("[I2C] ERROR: Read out of bounds (reg=0x%02X len=%zu)\n",
               reg, len);
        return false;
    }

    printf("[I2C] READ addr=0x%02X reg=0x%02X len=%zu\n",
           device_addr, reg, len);

    memcpy(data, &eeprom_memory[reg], len);

    return true;
}

// Mock I2C write function simulating EEPROM write behavior (not needed; for testing purposes)
bool i2c_write(uint8_t device_addr, uint8_t reg,
               const uint8_t *data, size_t len)
{
    eeprom_mock_init();

    if (data == NULL || len == 0)
        return false;

    if (device_addr != EEPROM_ADDR)
    {
        printf("[I2C] ERROR: Unknown device address 0x%02X\n", device_addr);
        return false;
    }

    if ((reg + len) > sizeof(eeprom_memory))
    {
        printf("[I2C] ERROR: Write out of bounds\n");
        return false;
    }

    printf("[I2C] WRITE addr=0x%02X reg=0x%02X len=%zu\n",
           device_addr, reg, len);

    memcpy(&eeprom_memory[reg], data, len);

    return true;
}