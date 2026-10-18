#include "global.h"
/* Math_ApproachShortArrayBySixteenth: Updates each signed halfword toward its target by one sixteenth with a minimum unit step */
#define Math_ApproachShortArrayBySixteenth sub_0819B37C

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

void sub_0819B37C(s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = Approach(*cur, *target);
        cur++;
        target++;
    }
}
