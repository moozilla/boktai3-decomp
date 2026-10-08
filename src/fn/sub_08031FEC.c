#include "global.h"

u32 sub_08031FEC(s32 *p, s32 lim)
{
    u32 n = 0;
    s32 v = *p;
    s32 r;
    if (v >= lim) {
        do {
            n++;
            r = v - lim;
            v = r;
        } while (r >= lim);
        *p = r;
    }
    return n;
}
