#include "global.h"
s32 sub_0819AF14(s32 a, s32 b)
{
    s32 d = b - a;
    s32 r;
    if (d != 0) {
        if ((u32)(d + 7) <= 14) {
            r = -8;
            if (d > 0) r = 8;
            d = r;
        }
        if (d >= 0) r = d >> 3;
        else r = -((-d) >> 3);
        b = a + r;
    }
    return b;
}
