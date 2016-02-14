#include <stdint.h>

#include <app.h>
#include <kernel/thread.h>
#include <platform/spi.h>

#include "matrix.h"
#include "seq.h"

static void robot_init(const struct app_descriptor *app) {
    matrix_init();
    seq_init();
}

static void robot_entry(const struct app_descriptor *app, void *args)
{
    while(true) {
        seq_frame();
        matrix_draw();
        thread_sleep(1);
    }
}

APP_START(robot)
    .init = robot_init,
    .entry = robot_entry,
APP_END

