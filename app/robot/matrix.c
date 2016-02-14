#include "matrix.h"

#include <compiler.h>
#include <stdio.h>
#include <string.h>

#include <platform/spi.h>

static uint32_t matrix_buf[WIDTH * HEIGHT + 2];

void matrix_init(void) {
    memset(matrix_buf, 0x0, sizeof(matrix_buf));
}

void matrix_set_pixel(uint32_t x, uint32_t y, matrix_color_t color) {
    uint i = y * WIDTH;
    if ((y & 0x1) == 0) {
        i += x;
    } else {
        i += WIDTH - 1 - x;
    }

    matrix_buf[i+1] = color;
}

matrix_color_t matrix_color(uint8_t r, uint8_t g, uint8_t b) {
    return 0xe3 | (b << 8) | (g << 16) | (r << 24);
}

void matrix_draw(void) {
    matrix_buf[0] = 0x00000000;
    matrix_buf[countof(matrix_buf) - 1] = 0xffffffff;
#if 0
    int x, y;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            //matrix_set_pixel(x, y, matrix_color(0xff, 0x00, 0x00));
                matrix_buf[y * WIDTH + x + 1] = matrix_color(0x00, 0xff, 0x00);
        }
    }
#endif
    spi_xfer(&matrix_buf, &matrix_buf, sizeof(matrix_buf));
}
