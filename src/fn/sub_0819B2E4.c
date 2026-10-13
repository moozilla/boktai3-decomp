#include "global.h"

static inline s32 Shift(s32 x)
{
    if (x >= 0)
        return x >> 2;
    return -((-x) >> 2);
}

static inline s32 Approach(s32 cur, s32 target)
{
    s32 d = target - cur;
    if (d == 0)
        return target;
    if ((u32)(d + (4 - 1)) <= 2 * (4 - 1))
        d = d > 0 ? 4 : -4;
    return cur + Shift(d);
}

void sub_0819B2E4(s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = Approach(*cur, *target);
        cur++;
        target++;
    }
}
