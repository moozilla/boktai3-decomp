#include "global.h"

s32 sub_0803FA5C(s32, s32, s32, s32, s32);

s32 sub_0803FAC8(s32 a, s32 b, s32 c, s32 d)
{
    c = sub_0803FA5C(a, b, c, d, 6);
    if (c > 999)
        c = 999;
    return c;
}
