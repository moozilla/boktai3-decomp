#include "global.h"
/* Math_ApproachSixteenth: Moves toward the target by one sixteenth of the difference with a minimum step of one */

static inline s32 Shift4(s32 x)
{
    if (x >= 0)
        return x >> 4;
    return -((-x) >> 4);
}

s32 Math_ApproachSixteenth(s32 cur, s32 target)
{
    s32 d = target - cur;
    if (d == 0)
        return target;
    if ((u32)(d + 15) <= 30)
        d = d > 0 ? 16 : -16;
    return cur + Shift4(d);
}
