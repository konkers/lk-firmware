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

        gpio_set(GPIO_NES_LATCH, 0);
        gpio_set(GPIO_NES_CLK, 1);

        gpio_config(GPIO_NES_LATCH, GPIO_OUTPUT);
        gpio_config(GPIO_NES_CLK, GPIO_OUTPUT);
        gpio_config(GPIO_NES_DATA, GPIO_INPUT);
}

void target_init(void)
{
	stm32_debug_init();

        spi_init(SPI_DATA_SIZE_8,
                 SPI_CPOL_LOW,
                 SPI_CPHA_1,
                 SPI_BIT_ORDER_MSB,
                 SPI_PRESCALER_64);

}

void target_set_debug_led(unsigned int led, bool on)
{
	switch (led) {
		case 0:
		        gpio_set(GPIO_LED0, on);
			break;
	}
}
