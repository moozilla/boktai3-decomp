#include "global.h"
struct Z { u32 x, y; };
void sub_080472A4(u8 *, s32, u32, u8 *, struct Z *, u32);
void sub_08063F48(u8 *, u32);
s32 sub_08063F84(u8 *p, u32 b, s32 c)
{
    u8 *q = *(u8 **)(p + 0x50);
    if (b == 0) {
        if (*(u16 *)(q + 0x13c) != 2) {
            if (c > 0) {
                struct Z z;
                u32 m = 0xFFFF0000;
                z.x = b;
                z.y &= m;
                sub_080472A4(q + 0xbc, c, 0, q + 0x28, &z, 1);
            }
            sub_08063F48(q, 1);
        }
    } else if (b == 1) {
        u32 *w;
        *(u16 *)(q + 0x13a) = c;
        w = (u32 *)(q + 0x124);
        w[1] |= b;
        sub_08063F48(q, 2);
    }
    return 1;
}
