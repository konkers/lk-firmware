#include "heart.h"

#include <stdint.h>

#include "heart.h"
#include "matrix.h"

static const uint8_t heart_data_big[] = {
    0x3c, 0x3c,   // ..XXXX.. ..XXXX..
    0x7e, 0x7e,   // .XXXXXX. .XXXXXX.
    0xff, 0xff,   // XXXXXXXX XXXXXXXX
    0xff, 0xff,   // XXXXXXXX XXXXXXXX

    0xff, 0xff,   // XXXXXXXX XXXXXXXX
    0xff, 0xff,   // XXXXXXXX XXXXXXXX
    0xff, 0xff,   // XXXXXXXX XXXXXXXX
    0xff, 0xff,   // XXXXXXXX XXXXXXXX

    0xff, 0xff,   // XXXXXXXX XXXXXXXX
    0x7f, 0xfe,   // .XXXXXXX XXXXXXX.
    0x3f, 0xfc,   // ..XXXXXX XXXXXX..
    0x1f, 0xf8,   // ...XXXXX XXXXX...

    0x0f, 0xf0,   // ....XXXX XXXX....
    0x07, 0xe0,   // .....XXX XXX.....
    0x03, 0xc0,   // ......XX XX......
    0x01, 0x80,   // .......X X.......
};

static const uint8_t heart_data_small[] = {
    0x00, 0x00,   // ........ ........
    0x00, 0x00,   // ........ ........
    0x1e, 0x78,   // ...XXXX. .XXXX...
    0x3f, 0xfc,   // ..XXXXXX XXXXXX..

    0x3f, 0xfc,   // ..XXXXXX XXXXXX..
    0x3f, 0xfc,   // ..XXXXXX XXXXXX..
    0x3f, 0xfc,   // ..XXXXXX XXXXXX..
    0x3f, 0xfc,   // ..XXXXXX XXXXXX..

    0x3f, 0xfc,   // ..XXXXXX XXXXXX..
    0x1f, 0xf8,   // ...XXXXX XXXXX...
    0x0f, 0xf0,   // ....XXXX XXXX....
    0x07, 0xe0,   // .....XXX XXX.....

    0x03, 0xc0,   // ......XX XX......
    0x01, 0x80,   // .......X X.......
    0x00, 0x00,   // ........ ........
    0x00, 0x00,   // ........ ........
};

uint32_t heart_frame_rate;
uint32_t heart_frame_counter;

const uint8_t *heart_frame_data;

static void heart_beat(void) {
    if (heart_frame_data == heart_data_big) {
        heart_frame_data = heart_data_small;
    } else {
        heart_frame_data = heart_data_big;
    }
    heart_frame_counter = 0;
}

void heart_init(void) {
    heart_frame_rate = 40;
    heart_frame_counter = 0;

    heart_frame_data = heart_data_big;
}

void heart_event(seq_button_t button) {
    switch (button) {
        case BUTTON_UP:
            if (heart_frame_rate > 1)
                heart_frame_rate--;
            break;

        case BUTTON_DOWN:
            heart_frame_rate++;
            break;

        case BUTTON_A:
            heart_beat();
            break;

        default:
            break;
    }
}

void heart_frame(void) {
    int x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            uint8_t mask = 1 << (7 - (x & 0x7));
            int index = (y * WIDTH + x) >> 3;
            if ((x < 16)  && (y < 16) && (heart_frame_data[index] & mask)) {
                matrix_set_pixel(x, y, matrix_color(0xff, 0x00, 0x00));
            } else {
                matrix_set_pixel(x, y, matrix_color(0x00, 0x00, 0x00));
            }
        }
    }

    if (++heart_frame_counter >= heart_frame_rate) {
        heart_beat();
    }
}
