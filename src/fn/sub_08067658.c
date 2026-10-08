#include "global.h"

void sub_08067658(u8 *p, u8 *a, u8 *b)
{
    u32 v = ((p[9] + 0x60) >> 6) & 3;
    if (v > 1) {
        *b = 1;
        *a = 3 - v;
    } else {
        *b = 0;
        *a = v;
    }
}
