#include "nes.h"

#include <rand.h>
#include <stdio.h>
#include <stdint.h>

#include <dev/gpio.h>
#include <platform.h>
#include <target/gpioconfig.h>

enum nes_button_mask {
    NES_BUTTON_A =      0x80,
    NES_BUTTON_B =      0x40,
    NES_BUTTON_SELECT = 0x20,
    NES_BUTTON_START =  0x10,
    NES_BUTTON_UP =     0x08,
    NES_BUTTON_DOWN =   0x04,
    NES_BUTTON_LEFT =   0x02,
    NES_BUTTON_RIGHT =  0x01,
};

uint8_t nes_last_state;

static void nes_gpio_set(unsigned gpio, unsigned on) {
    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);

    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);
    gpio_set(gpio, on);
}

static uint8_t nes_get_state(void) {
    uint8_t data = 0x0;
    int i;
    nes_gpio_set(GPIO_NES_LATCH, 1);
    nes_gpio_set(GPIO_NES_LATCH, 1);
    nes_gpio_set(GPIO_NES_LATCH, 0);
    for (i = 0; i < 8 ; i++) {
        data <<= 1;
        data |= gpio_get(GPIO_NES_DATA) ? 0x1 : 0x0;
        nes_gpio_set(GPIO_NES_CLK, 0);
        nes_gpio_set(GPIO_NES_CLK, 1);
    }
    return data;
}

static void nes_handle_button(uint8_t new_state, uint8_t mask,
                              seq_button_t button,
                              void (*handler)(seq_button_t button),
                              const char *name) {
    new_state &= mask;
    if (new_state != (nes_last_state & mask)) {
        if (!new_state) {
            printf("a");
           handler(button);
        }
        lk_bigtime_t time = current_time_hires();
        rand_add_entropy(&time, sizeof(time));
    }
}

void nes_init(void) {
    nes_last_state = 0xff;
}


void nes_process_events(void (*handler)(seq_button_t button)) {
    uint8_t new_state = nes_get_state();

    nes_handle_button(new_state, NES_BUTTON_UP, BUTTON_UP, handler, "up");
    nes_handle_button(new_state, NES_BUTTON_DOWN, BUTTON_DOWN, handler, "down");
    nes_handle_button(new_state, NES_BUTTON_LEFT, BUTTON_LEFT, handler, "left");
    nes_handle_button(new_state, NES_BUTTON_RIGHT, BUTTON_RIGHT, handler, "right");
    nes_handle_button(new_state, NES_BUTTON_A, BUTTON_A, handler, "a");
    nes_handle_button(new_state, NES_BUTTON_B, BUTTON_B, handler, "b");
    nes_handle_button(new_state, NES_BUTTON_SELECT, BUTTON_SELECT, handler, "select");
    nes_handle_button(new_state, NES_BUTTON_START, BUTTON_START, handler, "start");

    nes_last_state = new_state;
}

