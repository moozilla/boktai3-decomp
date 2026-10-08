#include "global.h"

struct P081594C8 { u8 f0[0x1c]; s32 f1c; u8 f20[0x424 - 0x20]; u16 f424; };

void sub_081594C8(struct P081594C8 *p, s32 d)
{
    if (p->f1c == 1) {
        if (p->f424 < d + 1)
            p->f424 = p->f1c;
        else
            p->f424 -= d;
    }
}
