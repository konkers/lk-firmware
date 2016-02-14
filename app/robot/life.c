#include "hsv.h"

#include <rand.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <platform.h>

#include "hsv.h"
#include "matrix.h"

#define BYTE_ALIVE

#ifdef BYTE_ALIVE
uint8_t life_alive[WIDTH * HEIGHT];
uint8_t life_last_alive[WIDTH * HEIGHT];
uint8_t life_new_alive[WIDTH * HEIGHT];
#else
uint8_t life_alive[(WIDTH * HEIGHT) / 8];
uint8_t life_last_alive[(WIDTH * HEIGHT) / 8];
uint8_t life_new_alive[(WIDTH * HEIGHT) / 8];
#endif
uint8_t life_age[WIDTH * HEIGHT];
uint8_t life_frame_num;
uint32_t life_color_index;

#ifndef RAND_MAX
#define RAND_MAX 0x7fffffff
#endif

static int life_wrap(int n) {
    if (n < 0) {
        n += WIDTH;
    }
    if (n >= WIDTH) {
        n -= WIDTH;
    }
    return n;
}


static void life_set(int x, int y, bool alive, uint8_t age) {
#ifdef BYTE_ALIVE
    life_new_alive[y * WIDTH + x] = alive;
#else
    uint8_t mask = 0x1 << (x & 0x7);
    uint8_t *data = &life_new_alive[(y * WIDTH + x) >> 3];

    if (alive) {
        *data |= mask;
    } else {
        *data &= ~mask;
    }
#endif

    life_age[(y * WIDTH + x)] = age;
}

static uint8_t life_get_age(int x, int y) {
    return life_age[y * WIDTH + x];
}

static bool life_is_alive(int x, int y) {
    x = life_wrap(x);
    y = life_wrap(y);

#ifdef BYTE_ALIVE
    return life_alive[y * WIDTH + x];
#else
    uint8_t mask = 0x1 << (x & 0x7);
    uint8_t *data = &life_new_alive[(y * WIDTH + x) >> 3];

    return *data & mask;
#endif
}

void life_init(void) {
    int x, y;
    srand(current_time());
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            life_set(x, y, rand() < (RAND_MAX / 5), 0);
        }
    }
    memcpy(life_alive, life_new_alive, sizeof(life_alive));
    memcpy(life_last_alive, life_alive, sizeof(life_alive));
    life_frame_num = 0;
}

void life_event(seq_button_t button) {
    switch (button) {
        case BUTTON_A:
            life_init();
            break;

        default:
            break;
    }
}

static int life_calc_neighbors(int x, int y) {
    int dx, dy;
    int num_alive = 0;
    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            if (life_is_alive(x + dx, y + dy)) {
                num_alive++;
            }
        }
    }
    return num_alive;
}

void life_frame(void) {
    int x, y;
    uint8_t sub_frame = life_frame_num++ & 0x1f;
    static bool reset_next = false;

    if (sub_frame == 0) {
        if (reset_next) {
            life_init();
            reset_next = false;
        }
        for (y = 0; y < HEIGHT; y++) {
            for (x = 0; x < WIDTH; x++) {
                bool alive = life_is_alive(x, y);
                uint8_t age = life_get_age(x, y);
                int neighbors = life_calc_neighbors(x, y);

                // Life rules from:
                //     https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
                // 1. Any live cell with fewer than two live neighbours dies, as if
                //    caused by under-population.
                // 2. Any live cell with two or three live neighbours lives on to
                //    the next generation.
                // 3. Any live cell with more than three live neighbours dies, as
                //    if by over-population.
                // 4. Any dead cell with exactly three live neighbours becomes a
                //    live cell, as if by reproduction.
                if (alive) {
                    if (neighbors == 2 || neighbors == 3) {
                        life_set(x, y, true, age < 0xff ? age + 1 : age);
                    } else {
                        life_set(x, y, false, 0);
                    }
                } else {
                    if (neighbors == 3) {
                        life_set(x, y, true, 0);
                    } else {
                        life_set(x, y, false, 0);
                    }
                }
            }
        }
        if (!memcmp(life_alive, life_new_alive, sizeof(life_alive))
            || !memcmp(life_last_alive, life_new_alive, sizeof(life_alive))) {
            reset_next = true;
        }
        memcpy(life_last_alive, life_alive, sizeof(life_alive));
        memcpy(life_alive, life_new_alive, sizeof(life_alive));
    }

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            uint32_t color;
            if (life_is_alive(x, y)) {
                int age = life_get_age(x, y);
                uint32_t index = hsv_inc(life_color_index, age * 0x20 * 4);
                color = hsv_pixel(index);
            } else {
                color = matrix_color(0, 0x00, 0x00);
            }
            matrix_set_pixel(x, y, color);
        }
    }
    life_color_index = hsv_inc(life_color_index, 4);
}
