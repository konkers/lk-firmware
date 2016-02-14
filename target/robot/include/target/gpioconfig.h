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
#ifndef __TARGET_GPIOCONFIG_H
#define __TARGET_GPIOCONFIG_H

#include <platform/gpio.h>

#define GPIO_LED0 GPIO(GPIO_PORT_A, 5)

#define GPIO_UART_TX    GPIO(GPIO_PORT_A, 2)
#define GPIO_UART_RX    GPIO(GPIO_PORT_A, 3)

#define GPIO_NES_LATCH  GPIO(GPIO_PORT_A, 5)
#define GPIO_NES_CLK    GPIO(GPIO_PORT_A, 6)
#define GPIO_NES_DATA   GPIO(GPIO_PORT_A, 7)

#define GPIO_UART1_TX    GPIO(GPIO_PORT_A, 9)
#define GPIO_UART1_RX    GPIO(GPIO_PORT_A, 10)

#define GPIO_SPI1_NCSS  GPIO(GPIO_PORT_A, 15)

#define GPIO_SPI1_SCK   GPIO(GPIO_PORT_B, 3)
#define GPIO_SPI1_MISO  GPIO(GPIO_PORT_B, 4)
#define GPIO_SPI1_MOSI  GPIO(GPIO_PORT_B, 5)

#define GPIO_CAN_RX     GPIO(GPIO_PORT_B, 8)
#define GPIO_CAN_TX     GPIO(GPIO_PORT_B, 9)
#endif
