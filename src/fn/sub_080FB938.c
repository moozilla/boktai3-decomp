#include "global.h"

struct S080FB938 { u8 filler[0x3d0]; u8 *q; };

void sub_080FB938(struct S080FB938 *s, u32 x)
{
    u8 *q = s->q;
    s32 i = 3;
    u8 *p = q + 0x860;
    do {
        *(u32 *)p = x;
        p -= 0x60;
    } while (--i >= 0);
}
