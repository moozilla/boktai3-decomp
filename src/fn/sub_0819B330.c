#include "global.h"

static inline s32 Shift(s32 x)
{
    if (x >= 0)
        return x >> 3;
    return -((-x) >> 3);
}

static inline s32 Approach(s32 cur, s32 target)
{
    s32 d = target - cur;
    if (d == 0)
        return target;
    if ((u32)(d + (8 - 1)) <= 2 * (8 - 1))
        d = d > 0 ? 8 : -8;
    return cur + Shift(d);
}

void sub_0819B330(s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = Approach(*cur, *target);
        cur++;
        target++;
    }
}
