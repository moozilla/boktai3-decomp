#include "global.h"

s32 DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);

s32 sub_0802D0F4(u8 *p, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    u32 dir, flip;
    s32 *q = (s32 *)(p + 0x184);
    if (*q == 1) {
        if (p[0x180] > 1) {
            dir = 3 - p[0x180];
            flip = 1;
        } else {
            dir = p[0x180];
            flip = 0;
        }
    } else {
        u8 v = p[0x180];
        u8 t = v - 1;
        dir = 0;
        if (t <= 1)
            dir = c;
        flip = 0;
        if (v > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(q, a, dir, flip, b, d, e);
}
