#include "global.h"

struct P08159494 { u8 f0[0x1c]; s32 f1c; u8 f20[0x424 - 0x20]; u16 f424; u16 f426; };

void sub_08159494(struct P08159494 *p, u32 d)
{
    if (p->f1c == 1) {
        p->f424 += d;
        if (p->f424 >= p->f426)
            p->f424 = p->f426;
    }
}
