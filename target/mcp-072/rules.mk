LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

STM32_CHIP := stm32f072_xB

PLATFORM := stm32f0xx

GLOBAL_DEFINES += \
	ENABLE_UART1=1 \
	ENABLE_UART2=1 \
	TARGET_HAS_DEBUG_LED=1 \
	HSE_VALUE=8000000 \
	PLL_M_VALUE=8 \
	PLL_N_VALUE=336 \
	PLL_P_VALUE=2

MODULE_SRCS += \
	$(LOCAL_DIR)/init.c \
	$(LOCAL_DIR)/usbd_cdc_interface.c \
	$(LOCAL_DIR)/usbd_conf.c \
	$(LOCAL_DIR)/usbd_desc.c

#	$(LOCAL_DIR)/usb.c

MODULE_DEPS += \
        platform/stm32f0xx/STM32F0xx_HAL_Driver \
        platform/stm32f0xx/STM32_USB_Device_Library/Class/CDC \
        platform/stm32f0xx/STM32_USB_Device_Library/Core

include make/module.mk

