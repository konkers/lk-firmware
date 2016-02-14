#ifndef __LIFE_H__
#define __LIFE_H__

#include "seq.h"

void life_init(void);
void life_event(seq_button_t button);
void life_frame(void);

#endif  // __LIFE_H__
