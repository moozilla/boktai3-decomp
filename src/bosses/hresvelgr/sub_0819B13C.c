#include "global.h"
/* Math_ApproachAngleBySixteenth: Approaches the target by one sixteenth of the shortest angular difference with a minimum unit step */

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
        return x >> 4;
    return -((-x) >> 4);
}

static inline s32 Approach(s32 cur, s32 target)
{
    s32 d = target - cur;
    if (d == 0)
        return target;
    if ((u32)(d + (16 - 1)) <= 2 * (16 - 1))
        d = d > 0 ? 16 : -16;
    return cur + Shift(d);
}

s32 Math_ApproachAngleBySixteenth(s32 angle, s32 target)
{
    return Approach(AngleNear(angle, target), target);
}
