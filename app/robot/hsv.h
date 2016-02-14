#ifndef __HSV_H__
#define __HSV_H__

#include <stdint.h>

#include "matrix.h"
#include "seq.h"

static inline uint32_t hsv_inc(uint32_t index, int16_t inc) {
    int32_t signed_index = index + inc;
    while (signed_index >= 0x600)
        signed_index -= 0x600;
    while (signed_index < 0)
        signed_index += 0x600;
    return signed_index;
}

static inline uint32_t hsv_pixel(uint32_t index) {
    uint8_t pos = index & 0xff;
    uint8_t neg = 0xff - (index & 0xff);
    uint8_t phase = (index >> 8) & 0x7;

    switch (phase) {
        case 0:
            return matrix_color(pos, 0x00, 0xff);

        case 1:
            return matrix_color(0xff, 0x00, neg);

        case 2:
            return matrix_color(0xff, pos, 0x00);

        case 3:
            return matrix_color(neg, 0xff, 0x00);

        case 4:
            return matrix_color(0x00, 0xff, pos);

        default:
        case 5:
            return matrix_color(0x00, neg, 0xff);
    }
}

void hsv_init(void);
void hsv_event(seq_button_t button);
void hsv_frame(void);

#endif  // __HSV_H__
