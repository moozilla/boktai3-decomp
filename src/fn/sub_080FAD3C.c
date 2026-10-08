#include "global.h"

void sub_080FAD3C(u16 x, s32 *a, s32 *b)
{
    s32 v = ((x + 0x70) >> 5) & 7;
    s32 r;
    if (v > 4) {
        *a = 8 - v;
        r = 1;
    } else {
        *a = v;
        r = 0;
    }
    *b = r;
}
