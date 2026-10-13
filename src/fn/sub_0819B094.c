#include "global.h"

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

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

s32 sub_0819B094(s32 angle, s32 target)
{
    return Approach(AngleNear(angle, target), target);
}
