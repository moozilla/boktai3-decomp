#include "global.h"

s32 DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);

s32 sub_0802D15C(u8 *p, u32 a, u32 b, u32 unused, u32 d, u32 e)
{
    u32 dir, flip;
    s32 *q = (s32 *)(p + 0x184);
    if (*q == 1) {
        dir = p[0x180];
        flip = 0;
        if (dir > 1)
            flip = 1;
    } else {
        u8 v;
        dir = 0;
        v = p[0x180];
        flip = 0;
        if (v > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(q, a, dir, flip, b, d, e);
}
