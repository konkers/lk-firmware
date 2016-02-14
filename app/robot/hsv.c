#include "hsv.h"

#include <stdint.h>

#include "matrix.h"

static int16_t y_inc;
static int16_t x_inc;
static int16_t frame_inc;

static uint32_t hsv_index;

void hsv_init(void) {
    y_inc = 0x600 / 64;
    x_inc = 0x600 / 64;
    frame_inc = 10;
    hsv_index = 0;
}

void hsv_event(seq_button_t button) {
    switch (button) {
        case BUTTON_UP:
            y_inc += 2;
            break;

        case BUTTON_DOWN:
            y_inc -= 2;
            break;

        case BUTTON_LEFT:
            x_inc -= 2;
            break;

        case BUTTON_RIGHT:
            x_inc += 2;
            break;

        default:
            break;
    }
}

void hsv_frame(void) {
    int x, y;

    for (y = 0; y < 16; y++) {
        uint32_t cur_index = hsv_inc(hsv_index, y * y_inc);
        for (x = 0; x < 16; x++) {
            matrix_set_pixel(x, y, hsv_pixel(cur_index));
            cur_index = hsv_inc(cur_index, x_inc);
        }
    }
    hsv_index = hsv_inc(hsv_index, frame_inc);
}
