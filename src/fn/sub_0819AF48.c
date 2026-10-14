#include "global.h"

static inline s32 Shift4(s32 x)
{
    if (x >= 0)
        return x >> 4;
    return -((-x) >> 4);
}

s32 sub_0819AF48(s32 cur, s32 target)
{
    s32 d = target - cur;
    if (d == 0)
        return target;
    if ((u32)(d + 15) <= 30)
        d = d > 0 ? 16 : -16;
    return cur + Shift4(d);
}
