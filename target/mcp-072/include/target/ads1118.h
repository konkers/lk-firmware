#ifndef __TARGET_ADS1118_H
#define __TARGET_ADS1118_H

#include <stdbool.h>
#include <stdint.h>

void ads1118_init(uint32_t cs_gpio, uint32_t miso_gpio);

void ads1118_start(void);
void ads1118_end(void);
bool ads1118_is_idle(void);

uint16_t ads1118_null_cycle(void);

uint16_t ads1118_start_internal_temp(void);
int32_t ads1118_convert_internal_temp(uint16_t raw_val);

uint16_t ads1118_start_tc0(void);
int32_t ads1118_convert_tc(int16_t val);

#endif  // __TARGET_ADS1118_H

