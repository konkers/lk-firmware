LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/heart.c \
	$(LOCAL_DIR)/hsv.c \
	$(LOCAL_DIR)/life.c \
	$(LOCAL_DIR)/matrix.c \
	$(LOCAL_DIR)/nes.c \
	$(LOCAL_DIR)/robot.c \
	$(LOCAL_DIR)/seq.c

include make/module.mk
