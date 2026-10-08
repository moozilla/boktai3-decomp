#include "global.h"

s32 sub_082217CC(s32 a, s32 b) {
    s32 d = b - a;
    s32 t;
    if (d != 0) {
        if ((u32)(d + 7) <= 14) {
            s32 k = -8;
            if (d > 0)
                k = 8;
            d = k;
        }
        if (d >= 0)
            t = d >> 3;
        else
            t = -(-d >> 3);
        b = a + t;
    }
    return b;
}
