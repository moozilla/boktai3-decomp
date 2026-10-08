#include "global.h"

s32 sub_08221798(s32 a, s32 b) {
    s32 d = b - a;
    s32 t;
    if (d != 0) {
        if ((u32)(d + 3) <= 6) {
            s32 k = -4;
            if (d > 0)
                k = 4;
            d = k;
        }
        if (d >= 0)
            t = d >> 2;
        else
            t = -(-d >> 2);
        b = a + t;
    }
    return b;
}
