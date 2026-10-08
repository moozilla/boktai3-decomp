#include "global.h"

struct P081594F4 { u8 f0[0x1c]; s32 f1c; u8 f20[0x428 - 0x20]; u16 f428; u16 f42A; };

void sub_081594F4(struct P081594F4 *p, u32 d)
{
    if (p->f1c == 1) {
        p->f428 += d;
        if (p->f428 >= p->f42A)
            p->f428 = p->f42A;
    }
}
