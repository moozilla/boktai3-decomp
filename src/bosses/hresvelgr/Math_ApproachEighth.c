#include "global.h"
s32 Math_ApproachEighth(s32 a, s32 b)
{
    s32 d = b - a;
    s32 r;
    if (d == 0)
        return b;
    if ((u32)(d + 7) <= 14)
        d = d > 0 ? 8 : -8;
    if (d >= 0)
        r = d >> 3;
    else
        r = -(-d >> 3);
    return a + r;
}
