#include "global.h"

void sub_080982E4(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    u32 *f = (u32 *)(q + 0x3D4);
    u8 *b = q;
    s32 i = 3;
    do {
        u16 *t = (u16 *)(b + 0x430);
        u16 v = *t;
        if (v != 0) {
            *t = v - 1;
            if (*t == 0)
                *f = *f | 1;
        }
        f = (u32 *)((u8 *)f + 0x60);
        b += 0x60;
        i--;
    } while (i >= 0);
}
