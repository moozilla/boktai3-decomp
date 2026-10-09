#include "global.h"
void sub_080A06C0(u16 x, s32 *a, s32 *b) {
    s32 v = ((x + 0x70) >> 5) & 7;
    s32 r;
    if (v <= 4) goto small;
    *a = 8 - v;
    r = 1;
    goto done;
small:
    *a = v;
    r = 0;
done:
    *b = r;
}
