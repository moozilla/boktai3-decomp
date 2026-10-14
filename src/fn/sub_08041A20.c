#include "global.h"

extern const u32 gUnk_086058D0[];

s32 sub_08041A20(u32 a, s32 b)
{
    s32 lim = gUnk_086058D0[a];
    if (b < 0)
        b = 0;
    else if (b >= lim)
        b = lim - 1;
    return b;
}
