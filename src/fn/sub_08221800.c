#include "global.h"

s32 sub_08221800(s32 a, s32 b) {
    s32 d = b - a;
    s32 t;
    if (d != 0) {
        if ((u32)(d + 15) <= 30) {
            s32 k = -16;
            if (d > 0)
                k = 16;
            d = k;
        }
        if (d >= 0)
            t = d >> 4;
        else
            t = -(-d >> 4);
        b = a + t;
    }
    return b;
}
