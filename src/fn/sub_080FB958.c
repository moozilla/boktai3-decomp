#include "global.h"

struct S080FB958 { u8 filler[0x3d0]; u8 *q; };

void sub_080FB958(struct S080FB958 *s, u32 x, u32 y)
{
    u8 *a = s->q + 0x740;
    u8 *b = s->q + 0x73a;
    s32 i = 3;
    do {
        *(u16 *)b = y;
        *(u32 *)a = x;
        a += 0x60;
        b += 0x60;
    } while (--i >= 0);
}
