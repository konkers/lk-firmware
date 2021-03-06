#include "seq.h"

#include <stdio.h>

#include <compiler.h>

#include "heart.h"
#include "hsv.h"
#include "life.h"

static seq_t seqs[] = {
    {
        .init = heart_init,
        .event = heart_event,
        .frame = heart_frame,
    },
    {
        .init = life_init,
        .event = life_event,
        .frame = life_frame,
    },
    {
        .init = hsv_init,
        .event = hsv_event,
        .frame = hsv_frame,
    },
};

static unsigned seq_index;

void seq_init(void) {
    seq_index = 0;

    seqs[seq_index].init();
}

void seq_event(seq_button_t button) {
    switch (button) {
        case BUTTON_SELECT:
            seq_index++;
            if (seq_index == countof(seqs))
                seq_index = 0;
            seqs[seq_index].init();

        default:
            seqs[seq_index].event(button);
    }
}

void seq_frame(void) {
    seqs[seq_index].frame();
}
