#ifndef __SEQ_H__
#define __SEQ_H__

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_A,
    BUTTON_B,
    BUTTON_SELECT,
    BUTTON_START,
} seq_button_t;

typedef struct {
    void (* init)(void);
    void (* event)(seq_button_t button);
    void (* frame)(void);
} seq_t;

void seq_init(void);
void seq_event(seq_button_t button);
void seq_frame(void);

#endif  // __SEQ_H__
