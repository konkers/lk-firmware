#ifndef __TARGET_ADS1118_H
#define __TARGET_ADS1118_H

#include <stdint.h>

void ads1118_init(uint32_t cs_gpio, uint32_t miso_gpio);
int32_t ads1118_get_internal_temp(void);

#endif  // __TARGET_ADS1118_H

