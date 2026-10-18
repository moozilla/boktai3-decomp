#include "global.h"
typedef struct {
    u8 pad[0xE50];
    s16 scale, min, max, step;
} Actor;
void sub_081F2D80(Actor *p)
{
    p->scale += p->step;
    if (p->step > 0) {
        if (p->scale >= p->max) {
            p->scale = p->max;
            p->step = -32;
        }
    } else {
        if (p->scale <= p->min) {
            p->scale = p->min;
            p->step = 32;
        }
    }
}
