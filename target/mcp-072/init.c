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
#include <kernel/thread.h>
#include <platform/can.h>
#include <platform/spi.h>
#include <platform/stm32.h>
#include <target/gpioconfig.h>
#include <lib/console.h>
#include <stdbool.h>
#include <string.h>
#include <app.h>
#if 0
#include <platform/usbc.h>
#endif

#include <stm32f0xx_hal.h>
#include <usbd_cdc_interface.h>
#include <usbd_core.h>
#include <usbd_desc.h>

USBD_HandleTypeDef USBD_Device;

void target_usb_setup(void);
#define USE_USB_CLKSOURCE_PLL
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
#if defined (USE_USB_CLKSOURCE_CRSHSI48)
  static RCC_CRSInitTypeDef RCC_CRSInitStruct;
#endif
  
#if defined (USE_USB_CLKSOURCE_CRSHSI48)
  
  /* Enable HSI48 Oscillator to be used as system clock source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  HAL_RCC_OscConfig(&RCC_OscInitStruct); 
  
  /* Select HSI48 as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Select HSI48 as system clock source and configure the HCLK and PCLK1 
  clock dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  
  /*Configure the clock recovery system (CRS)**********************************/
  
  /*Enable CRS Clock*/
  __HAL_RCC_CRS_CLK_ENABLE(); 
  
  /* Default Synchro Signal division factor (not divided) */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  
  /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  
  /* HSI48 is synchronized with USB SOF at 1KHz rate */
  RCC_CRSInitStruct.ReloadValue =  __HAL_RCC_CRS_CALCULATE_RELOADVALUE(48000000, 1000);
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;
  
  /* Set the TRIM[5:0] to the default value*/
  RCC_CRSInitStruct.HSI48CalibrationValue = 0x20; 
  
  /* Start automatic synchronization */ 
  HAL_RCCEx_CRSConfig (&RCC_CRSInitStruct);
  
#elif defined (USE_USB_CLKSOURCE_PLL)
  
  /* Enable HSE Oscillator and activate PLL with HSE as source
  PLLCLK = (8 * 6) / 1) = 48 MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct); 
  
  /*Select PLL 48 MHz output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLCLK;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK and PCLK1 
  clock dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  
#endif /*USE_USB_CLKSOURCE_CRSHSI48*/
  
}


void target_early_init(void)
{
    HAL_Init();
    SystemClock_Config();

	/* configure the usart1 pins */
	gpio_config(GPIO_UART_TX, GPIO_STM32_AF | GPIO_STM32_AFn(1));
	gpio_config(GPIO_UART_RX, GPIO_STM32_AF | GPIO_STM32_AFn(1));

        // XXX: tmp
	gpio_config(GPIO_UART1_TX, GPIO_STM32_AF | GPIO_STM32_AFn(1));
	gpio_config(GPIO_UART1_RX, GPIO_STM32_AF | GPIO_STM32_AFn(1));

        /* configure can pins */
	gpio_config(GPIO_CAN_RX, GPIO_STM32_AF | GPIO_STM32_AFn(4));
	gpio_config(GPIO_CAN_TX, GPIO_STM32_AF | GPIO_STM32_AFn(4));

        /* configure spi pins */
#if 1
	gpio_config(GPIO_SPI1_SCK, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_config(GPIO_SPI1_MISO, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_config(GPIO_SPI1_MOSI, GPIO_STM32_AF | GPIO_STM32_AFn(0));
	gpio_set(GPIO_SPI1_NCSS, 1);
#endif
	gpio_config(GPIO_SPI1_NCSS, GPIO_OUTPUT);

	stm32_debug_early_init();

	/* configure some status leds */
	gpio_set(GPIO_LED0, 1);
	gpio_config(GPIO_LED0, GPIO_OUTPUT);
	gpio_set(GPIO_LED1, 1);
	gpio_config(GPIO_LED1, GPIO_OUTPUT);

        //stm32_usbc_early_init(STM32_USB_CLK_PLL);
        //
}

void target_init(void)
{
	stm32_debug_init();
        //stm32_usbc_init();
#if 0
        spi_init(SPI_DATA_SIZE_8,
                 SPI_CPOL_LOW,
                 SPI_CPHA_1,
                 SPI_BIT_ORDER_MSB,
                 SPI_PRESCALER_64);
#endif
#if 0
        target_usb_setup();
#endif

        can_init(true);

  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, &USBD_CDC);
  
  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
  
  /* Start Device Process */
  USBD_Start(&USBD_Device);
}

void target_set_debug_led(unsigned int led, bool on)
{
	switch (led) {
		case 0:
		        gpio_set(GPIO_LED0, !on);
			break;
	}
}

#if 0
static void blink_entry(const struct app_descriptor *app, void *args)
{
    while(true) {
        gpio_set(GPIO_LED1, false);
        thread_sleep(1000);
        gpio_set(GPIO_LED1, true);
        thread_sleep(1000);
    }
}

APP_START(blink)
    .entry = blink_entry,
APP_END

#endif

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


    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("can_test", "can tests", &cmd_can_test)
STATIC_COMMAND_END(mcp);

#if 0
static void can_entry(const struct app_descriptor *app, void *args)
{
    while (1) {
        can_msg_t msg;
        int i;

        for (i = 0; i < 8; i++) {
            msg.data[i] = 0xff;
        }

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
#endif
