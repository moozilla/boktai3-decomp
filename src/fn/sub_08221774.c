#include "global.h"

s32 sub_08221774(s32 a, s32 b) {
    s32 d = b - a;
    s32 t;
    if ((u32)(d + 1) <= 2)
        return b;
    if (d >= 0)
        t = d >> 1;
    else
        t = -(-d >> 1);
    return a + t;
}
