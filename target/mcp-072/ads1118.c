#include <target/ads1118.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <compiler.h>
#include <dev/gpio.h>
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

typedef struct {
    int16_t adc_val;
    int32_t temp;
} ads1118_tc_conv;

static const ads1118_tc_conv ads1118_tc_table[] = {
    {.adc_val = 0x0000, .temp = 0x00000},  // 0.000000mV == -0.000000C.
{.adc_val = 0x002d, .temp = 0x008f9},  // 0.355882mV == 8.972707C.
{.adc_val = 0x005b, .temp = 0x011db},  // 0.711763mV == 17.856471C.
{.adc_val = 0x0088, .temp = 0x01aa9},  // 1.067645mV == 26.662306C.
{.adc_val = 0x00b6, .temp = 0x02366},  // 1.423527mV == 35.400834C.
{.adc_val = 0x00e3, .temp = 0x02c15},  // 1.779408mV == 44.082685C.
{.adc_val = 0x0111, .temp = 0x034b8},  // 2.135290mV == 52.718753C.
{.adc_val = 0x013e, .temp = 0x03d51},  // 2.491172mV == 61.320305C.
{.adc_val = 0x016c, .temp = 0x045e6},  // 2.847054mV == 69.898967C.
{.adc_val = 0x0199, .temp = 0x04e77},  // 3.202935mV == 78.466568C.
{.adc_val = 0x01c7, .temp = 0x05708},  // 3.558817mV == 87.034873C.
{.adc_val = 0x01f5, .temp = 0x05f9d},  // 3.914699mV == 95.615211C.
{.adc_val = 0x0222, .temp = 0x06837},  // 4.270580mV == 104.218020C.
{.adc_val = 0x0250, .temp = 0x070da},  // 4.626462mV == 112.852330C.
{.adc_val = 0x027d, .temp = 0x07986},  // 4.982344mV == 121.525231C.
{.adc_val = 0x02ab, .temp = 0x0823d},  // 5.338225mV == 130.241372C.
{.adc_val = 0x02d8, .temp = 0x08b00},  // 5.694107mV == 139.002573C.
{.adc_val = 0x0306, .temp = 0x093ce},  // 6.049989mV == 147.807604C.
{.adc_val = 0x0333, .temp = 0x09ca6},  // 6.405870mV == 156.652224C.
{.adc_val = 0x0361, .temp = 0x0a587},  // 6.761752mV == 165.529499C.
{.adc_val = 0x038f, .temp = 0x0ae6e},  // 7.117634mV == 174.430405C.
{.adc_val = 0x03bc, .temp = 0x0b758},  // 7.473516mV == 183.344643C.
{.adc_val = 0x03ea, .temp = 0x0c042},  // 7.829397mV == 192.261555C.
{.adc_val = 0x0417, .temp = 0x0c92b},  // 8.185279mV == 201.171017C.
{.adc_val = 0x0445, .temp = 0x0d210},  // 8.541161mV == 210.064162C.
{.adc_val = 0x0472, .temp = 0x0daef},  // 8.897042mV == 218.933870C.
{.adc_val = 0x04a0, .temp = 0x0e3c6},  // 9.252924mV == 227.774999C.
{.adc_val = 0x04cd, .temp = 0x0ec95},  // 9.608806mV == 236.584366C.
{.adc_val = 0x04fb, .temp = 0x0f55c},  // 9.964687mV == 245.360547C.
{.adc_val = 0x0529, .temp = 0x0fe1a},  // 10.320569mV == 254.103564C.
{.adc_val = 0x0556, .temp = 0x106d0},  // 10.676451mV == 262.814521C.
{.adc_val = 0x0584, .temp = 0x10f7e},  // 11.032333mV == 271.495251C.
{.adc_val = 0x05b1, .temp = 0x11825},  // 11.388214mV == 280.147993C.
{.adc_val = 0x05df, .temp = 0x120c6},  // 11.744096mV == 288.775138C.
{.adc_val = 0x060c, .temp = 0x12961},  // 12.099978mV == 297.379036C.
{.adc_val = 0x063a, .temp = 0x131f6},  // 12.455859mV == 305.961868C.
{.adc_val = 0x0667, .temp = 0x13a86},  // 12.811741mV == 314.525572C.
{.adc_val = 0x0695, .temp = 0x14312},  // 13.167623mV == 323.071816C.
{.adc_val = 0x06c3, .temp = 0x14b9a},  // 13.523504mV == 331.601997C.
{.adc_val = 0x06f0, .temp = 0x1541e},  // 13.879386mV == 340.117272C.
{.adc_val = 0x071e, .temp = 0x15c9e},  // 14.235268mV == 348.618589C.
{.adc_val = 0x074b, .temp = 0x1651b},  // 14.591149mV == 357.106734C.
{.adc_val = 0x0779, .temp = 0x16d95},  // 14.947031mV == 365.582368C.
{.adc_val = 0x07a6, .temp = 0x1760b},  // 15.302913mV == 374.046065C.
{.adc_val = 0x07d4, .temp = 0x17e7f},  // 15.658795mV == 382.498346C.
{.adc_val = 0x0801, .temp = 0x186f0},  // 16.014676mV == 390.939706C.
{.adc_val = 0x082f, .temp = 0x18f5e},  // 16.370558mV == 399.370626C.
{.adc_val = 0x085c, .temp = 0x197ca},  // 16.726440mV == 407.791596C.
{.adc_val = 0x088a, .temp = 0x1a033},  // 17.082321mV == 416.203120C.
{.adc_val = 0x08b8, .temp = 0x1a89b},  // 17.438203mV == 424.605722C.
{.adc_val = 0x08e5, .temp = 0x1b0ff},  // 17.794085mV == 432.999949C.
{.adc_val = 0x0913, .temp = 0x1b962},  // 18.149966mV == 441.386377C.
{.adc_val = 0x0940, .temp = 0x1c1c3},  // 18.505848mV == 449.765605C.
{.adc_val = 0x096e, .temp = 0x1ca23},  // 18.861730mV == 458.138255C.
{.adc_val = 0x099b, .temp = 0x1d281},  // 19.217611mV == 466.504975C.
{.adc_val = 0x09c9, .temp = 0x1dadd},  // 19.573493mV == 474.866431C.
{.adc_val = 0x09f6, .temp = 0x1e339},  // 19.929375mV == 483.223309C.
{.adc_val = 0x0a24, .temp = 0x1eb93},  // 20.285257mV == 491.576310C.
{.adc_val = 0x0a52, .temp = 0x1f3ed},  // 20.641138mV == 499.926149C.
{.adc_val = 0x0a7f, .temp = 0x1fc46},  // 20.997020mV == 508.273553C.
{.adc_val = 0x0aad, .temp = 0x2049e},  // 21.352902mV == 516.619258C.
{.adc_val = 0x0ada, .temp = 0x20cf6},  // 21.708783mV == 524.964007C.
{.adc_val = 0x0b08, .temp = 0x2154e},  // 22.064665mV == 533.308546C.
{.adc_val = 0x0b35, .temp = 0x21da7},  // 22.420547mV == 541.653627C.
};

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

    return in[1] | (in[0] << 8);

    // TODO(konkers): This should block on a GPIO interrupt.
    while(gpio_get(ads1118_miso_gpio)) {}

   // gpio_config(ads1118_miso_gpio, GPIO_STM32_AF | GPIO_STM32_AFn(0));

    out[0] = 0x00;
    out[1] = 0x00;
    spi_xfer(out, in, 2);

    gpio_set(ads1118_cs_gpio, 1);
}

void ads1118_start(void) {
    gpio_set(ads1118_cs_gpio, 0);
}

void ads1118_end(void) {
    gpio_set(ads1118_cs_gpio, 1);
}

bool ads1118_is_idle(void) {
    return !gpio_get(ads1118_miso_gpio);
}

uint16_t ads1118_null_cycle(void) {
    return ads1118_cycle(0);
}

uint16_t ads1118_start_internal_temp(void) {
    return ads1118_cycle(
        ADS1118_CFG_RESERVED |
        ADS1118_CFG_NOP_VALID |
        ADS1118_CFG_PULL_UP_EN |
        ADS1118_CFG_TS_MODE_TEMP |
        ADS1118_CFG_DR_64_SPS |
        ADS1118_CFG_MODE_SINGLE |
        ADS1118_CFG_SS);
}

int32_t ads1118_convert_internal_temp(uint16_t raw_val) {
    // Internal temperature in Q9.7 format.
    uint32_t val = raw_val;

    // Sign extend Q9.7 to Q25.7.
    if (val & (1<<16)) {
        val |= 0xffffffff << 16;
    }

    // Convert from Q25.7 to Q24.8
    val *= 2;

    return (int32_t)val;
}

uint16_t ads1118_start_tc0(void) {
    return ads1118_cycle(
        ADS1118_CFG_RESERVED |
        ADS1118_CFG_NOP_VALID |
        ADS1118_CFG_PULL_UP_EN |
        ADS1118_CFG_TS_MODE_ADC |
        ADS1118_CFG_DR_64_SPS |
        ADS1118_CFG_MODE_SINGLE |
        ADS1118_CFG_MUX_DIFF_0_1 |
        ADS1118_CFG_PGA_0_256_V |
        ADS1118_CFG_SS);
}

int32_t ads1118_convert_tc(int16_t val) {
    int num_entries = countof(ads1118_tc_table);
    assert(num_entries > 1);

    int32_t sign = 1;
    if (val < 0) {
        sign = -1;
        val = -val;
    }

    for (int i = 1; i < num_entries; i++) {
        ads1118_tc_conv a = ads1118_tc_table[i - 1];
        ads1118_tc_conv b = ads1118_tc_table[i];
        if (a.adc_val <= val && val < b.adc_val) {


            int32_t step_adc = b.adc_val - a.adc_val;
            int32_t step_temp = b.temp - a.temp;
            int32_t delta_adc = val - a.adc_val;

            int32_t temp = a.temp + step_temp * delta_adc / step_adc;
            return sign * temp;
        }
    }

    return sign * ads1118_tc_table[num_entries - 1].temp;
}

