#include <target/ads1118.h>

#include <stdint.h>
#include <stdio.h>

#include <dev/gpio.h>
#include <kernel/thread.h>
#include <platform/gpio.h>
#include <platform/spi.h>

// Reserved must be written as 1.
#define ADS1118_CFG_RESERVED  (1 << 0)

#define ADS1118_CFG_NOP_NOP0  (0 << 1)
#define ADS1118_CFG_NOP_VALID (1 << 1)
#define ADS1118_CFG_NOP_NOP2  (2 << 1)
#define ADS1118_CFG_NOP_NOP3  (3 << 1)

#define ADS1118_CFG_PULL_UP_EN (1 << 3)

#define ADS1118_CFG_TS_MODE_ADC  (0 << 4)
#define ADS1118_CFG_TS_MODE_TEMP (1 << 4)

#define ADS1118_CFG_DR_8_SPS   (0 << 5)
#define ADS1118_CFG_DR_16_SPS  (1 << 5)
#define ADS1118_CFG_DR_32_SPS  (2 << 5)
#define ADS1118_CFG_DR_64_SPS  (3 << 5)
#define ADS1118_CFG_DR_128_SPS (4 << 5)
#define ADS1118_CFG_DR_250_SPS (5 << 5)
#define ADS1118_CFG_DR_475_SPS (6 << 5)
#define ADS1118_CFG_DR_860_SPS (7 << 5)

#define ADS1118_CFG_MODE_CONTINUOUS (0 << 8)
#define ADS1118_CFG_MODE_SINGLE     (1 << 8)

#define ADS1118_CFG_PGA_6_114_V   (0 << 9)
#define ADS1118_CFG_PGA_4_096_V   (1 << 9)
#define ADS1118_CFG_PGA_2_048_V   (2 << 9)
#define ADS1118_CFG_PGA_1_023_V   (3 << 9)
#define ADS1118_CFG_PGA_0_512_V   (4 << 9)
#define ADS1118_CFG_PGA_0_256_V   (5 << 9)
#define ADS1118_CFG_PGA_0_256_V_2 (6 << 9)
#define ADS1118_CFG_PGA_0_256_V_3 (7 << 9)

#define ADS1118_CFG_MUX_DIFF_0_1 (0 << 12)
#define ADS1118_CFG_MUX_DIFF_0_3 (1 << 12)
#define ADS1118_CFG_MUX_DIFF_1_3 (2 << 12)
#define ADS1118_CFG_MUX_DIFF_2_3 (3 << 12)
#define ADS1118_CFG_MUX_0        (4 << 12)
#define ADS1118_CFG_MUX_1        (5 << 12)
#define ADS1118_CFG_MUX_2        (6 << 12)
#define ADS1118_CFG_MUX_3        (7 << 12)

#define ADS1118_CFG_SS (1 << 15)

static uint32_t ads1118_cs_gpio;
static uint32_t ads1118_miso_gpio;

void ads1118_init(uint32_t cs_gpio, uint32_t miso_gpio) {
    ads1118_cs_gpio = cs_gpio;
    ads1118_miso_gpio = miso_gpio;
    spi_init(SPI_DATA_SIZE_8,
             SPI_CPOL_LOW,
             SPI_CPHA_1,
             SPI_BIT_ORDER_MSB,
             SPI_PRESCALER_16);
}

static uint16_t ads1118_cycle(uint16_t cfg_reg) {
    uint8_t out[4];
    uint8_t in[4];

    gpio_set(ads1118_cs_gpio, 0);

    out[0] = cfg_reg >> 8;
    out[1] = cfg_reg & 0xff;
    out[2] = 0x00;
    out[3] = 0x00;
    spi_xfer(out, in, 4);

    gpio_config(ads1118_miso_gpio, GPIO_INPUT);

    // TODO(konkers): This should block on a GPIO interrupt.
    while(gpio_get(ads1118_miso_gpio)) {}

    gpio_config(ads1118_miso_gpio, GPIO_STM32_AF | GPIO_STM32_AFn(0));

    out[0] = 0x00;
    out[1] = 0x00;
    spi_xfer(out, in, 2);

    gpio_set(ads1118_cs_gpio, 1);
    return in[1] | (in[0] << 8);
}

int32_t ads1118_get_internal_temp(void) {
    // Internal temperature in Q9.7 format.
    uint32_t val = ads1118_cycle(
        ADS1118_CFG_RESERVED |
        ADS1118_CFG_NOP_VALID |
        ADS1118_CFG_PULL_UP_EN |
        ADS1118_CFG_TS_MODE_TEMP |
        ADS1118_CFG_DR_64_SPS |
        ADS1118_CFG_MODE_SINGLE |
        ADS1118_CFG_SS);
    // Sign extend Q9.7 to Q25.7.
    if (val & (1<<16)) {
        val |= 0xffffffff << 16;
    }

    // Convert from Q25.7 to Q24.8
    val <<= 1;

    return val;
}
