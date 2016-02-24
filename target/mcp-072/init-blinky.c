/*
 * Copyright (c) 2012 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <err.h>
#include <debug.h>
#include <target.h>
#include <compiler.h>
#include <dev/gpio.h>
#include <dev/uart.h>
#include <platform/gpio.h>
#include <stm32f0xx_usart.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_flash.h>
#include <stm32f0xx_dbgmcu.h>
#include <kernel/thread.h>
#include <platform/can.h>
#include <platform/spi.h>
#include <platform/stm32.h>
#include <target/gpioconfig.h>
#include <lib/console.h>
#include <stdbool.h>
#include <string.h>
#include <app.h>

void target_early_init(void)
{
	/* configure the usart2 pins */
	gpio_config(GPIO_UART_TX, GPIO_STM32_AF | GPIO_STM32_AFn(1));
	gpio_config(GPIO_UART_RX, GPIO_STM32_AF | GPIO_STM32_AFn(1));

	gpio_config(GPIO_UART1_TX, GPIO_STM32_AF | GPIO_STM32_AFn(1));
	gpio_config(GPIO_UART1_RX, GPIO_STM32_AF | GPIO_STM32_AFn(1));

        /* configure can pins */
	gpio_config(GPIO_CAN_RX, GPIO_STM32_AF | GPIO_STM32_AFn(4));
	gpio_config(GPIO_CAN_TX, GPIO_STM32_AF | GPIO_STM32_AFn(4));

        /* configure spi pins */
	gpio_config(GPIO_SPI1_SCK, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_config(GPIO_SPI1_MISO, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_config(GPIO_SPI1_MOSI, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_set(GPIO_SPI1_NCSS, 1);
	gpio_config(GPIO_SPI1_NCSS, GPIO_OUTPUT);

	stm32_debug_early_init();

	/* configure some status leds */
	gpio_set(GPIO_LED0, 1);
	gpio_config(GPIO_LED0, GPIO_OUTPUT);
}

void target_init(void)
{
	stm32_debug_init();

        can_init(true);


#if 0
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        SPI_InitTypeDef init;
        init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        init.SPI_DataSize = SPI_DataSize_16b;
        init.SPI_CPOL = SPI_CPOL_Low;
        init.SPI_CPHA = SPI_CPHA_1Edge;
        init.SPI_NSS = SPI_NSS_Soft;
        init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
        init.SPI_FirstBit = SPI_FirstBit_MSB;
        init.SPI_CRCPolynomial = 7;
        init.SPI_Mode = SPI_Mode_Master;

        SPI_Init(SPI1, &init);
        /* Initialize the FIFO threshold */
        SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

#if 0
        /* Enable the Rx buffer not empty interrupt */
        SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
        /* Enable the SPI Error interrupt */
        SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_ERR, ENABLE);
        /* Data transfer is performed in the SPI interrupt routine */
#endif

        /* Enable the SPI peripheral */
        SPI_Cmd(SPI1, ENABLE);
#else
        spi_init(SPI_DATA_SIZE_8,
                 SPI_CPOL_LOW,
                 SPI_CPHA_1,
                 SPI_BIT_ORDER_MSB,
                 SPI_PRESCALER_64);
#endif


}

void target_set_debug_led(unsigned int led, bool on)
{
	switch (led) {
		case 0:
		        gpio_set(GPIO_LED0, on);
			break;

	}
}

static int cmd_dmx_test(int argc, const cmd_args *argv) {

    int j;
    for (j = 0; j<10000; j++) {
        int i;
        gpio_config(GPIO_UART1_TX, GPIO_OUTPUT);
        for(i = 0; i< 100; i++) {
            gpio_set(GPIO_UART1_TX, 0);
        }
        for(i = 0; i< 4; i++) {
            gpio_set(GPIO_UART1_TX, 1);
        }
        gpio_config(GPIO_UART1_TX, GPIO_STM32_AF | GPIO_STM32_AFn(1));
        uart_putc(1, 0x00);
        for (i = 0 ; i<20; i++){
            uart_putc(1, 0xc0);
            uart_putc(1, 0xf0);
            uart_putc(1, 0xf0);
            uart_putc(1, 0x0f);
            uart_putc(1, 0x0f);
           // uart_putc(1, 0xff);
           // uart_putc(1, 0xff);
           // uart_putc(1, 0xff);
           // uart_putc(1, 0xff);
        }
        thread_sleep(1);
    }


    return 0;

}
static uint32_t gfx_buf[32 * 32 + 2];

static uint32_t pixel(uint8_t r, uint8_t g, uint8_t b) {
    return 0xe3 | (b << 8) | (g << 16) | (r << 24);
}
static int cmd_spi_test(int argc, const cmd_args *argv) {

#if 0
    SPI_I2S_SendData16(SPI1, 0xa5a5);
    while (((SPI1->SR >> 9 & 0x3) < 2)) {}
    uint16_t data = SPI_I2S_ReceiveData16(SPI1);
    printf("%04x\n", data);
#else
    uint i;
    gfx_buf[0] = 0x00000000;
    for (i = 0; i < (32 * 32); i++) {
        gfx_buf[i+1] = pixel(0xff, 0xff, 0xff);
    }
    gfx_buf[countof(gfx_buf) - 1] = 0xffffffff;

    spi_xfer(&gfx_buf, &gfx_buf, sizeof(gfx_buf));
#endif
    return 0;
}

static int cmd_can_test(int argc, const cmd_args *argv) {
    can_msg_t msg;
    int i;

    msg.id = 0x5a;
    msg.ide = 0;
    msg.rtr = 0;
    msg.dlc = 6;
    for (i = 0; i < 8; i++) {
        msg.data[i] = i;
    }
    can_send(&msg);

    for (i = 0; i < 8; i++) {
        msg.data[i] = 0xff;
    }


    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("can_test", "can tests", &cmd_can_test)
STATIC_COMMAND("dmx_test", "dmx tests", &cmd_dmx_test)
STATIC_COMMAND("spi_test", "spi tests", &cmd_spi_test)
STATIC_COMMAND_END(mcp);

static void can_entry(const struct app_descriptor *app, void *args)
{
    while (1) {
        can_msg_t msg;
        int i;

        can_recv(&msg, true);
        printf("%03x, %d, %d,", msg.id, msg.rtr, msg.dlc);
        for (i = 0; i < msg.dlc; i++) {
            printf(" %02x", msg.data[i]);
        }
        printf("\n");
    }
}

APP_START(can)
        .entry = can_entry,
APP_END
