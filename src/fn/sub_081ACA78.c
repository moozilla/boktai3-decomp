#include "global.h"

struct A { u8 f[0x9c]; u32 fl; u8 g[0x11]; u8 b1; u8 h[0x506]; u16 x; u16 y; };
void sub_08021890(void *, void *, void *, s32, s32, s32);

void sub_081ACA78(struct A *p)
{
    u32 m = 4;
    p->fl |= m;
    if (p->b1 != 0) {
        p->b1 = 0;
        p->x = 0;
        p->y = 0;
        sub_08021890(p, (u8 *)p + 0x18c, (u8 *)p + 0x1ec, 0xf, 0, 4);
    }
}
