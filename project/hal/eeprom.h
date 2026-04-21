#ifndef EEPROM_H
#define EEPROM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HW_REV_A = 0,
    HW_REV_B = 1,
} hw_revision_t;

typedef struct {
    hw_revision_t revision;
    char serial[16];
} config_t;

bool eeprom_read_config(config_t *config);

#ifdef __cplusplus
}
#endif

#endif // EEPROM_H
