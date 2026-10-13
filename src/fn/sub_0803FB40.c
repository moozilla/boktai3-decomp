#include "global.h"

s32 sub_0803FA5C(s32, s32, s32, s32, s32);

s32 sub_0803FB40(s32 a, s32 b, s32 c, s32 d)
{
    s32 r;
    if (c <= 0) {
        r = 0;
    } else {
        c = sub_0803FA5C(a, b, c, d, 3);
        if (c > 999)
            c = 999;
        r = c;
    }
    return r;
}
