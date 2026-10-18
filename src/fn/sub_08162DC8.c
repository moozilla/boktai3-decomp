#include "global.h"

struct P {
    u8 f00[0xb8];
    s32 st;
    u8 f[0xf4 - 0xbc];
    u32 fl;
};

void PlayerBarrier_Hide(struct P *p)
{
    if (p != 0) {
        p->st = 0;
        p->fl |= 1;
    }
}
