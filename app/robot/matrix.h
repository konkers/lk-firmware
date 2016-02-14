#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <stdint.h>

#define WIDTH 16
#define HEIGHT 16

typedef uint32_t matrix_color_t;

void matrix_init(void);
void matrix_set_pixel(uint32_t x, uint32_t y, matrix_color_t color);
matrix_color_t matrix_color(uint8_t r, uint8_t g, uint8_t b);
void matrix_draw(void);

#endif  // __MATRIX_H__
