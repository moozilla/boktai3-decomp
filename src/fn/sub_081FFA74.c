#include "global.h"
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x43c - 0x1c]; s32 cnt; };
struct H { u8 f[6]; u16 h; };
void sub_08214514(void *);
void sub_081FFA74(struct A *a, u8 *b, u32 n)
{
    u32 m, t;
    u32 *q;
    struct H *s;
    a->cnt--;
    s = (struct H *)(b + 0x78);
    s->h = 4 | s->h;
    q = (u32 *)(b + 0x10c);
    t = *q; m = 1; *q = t | m;
    sub_08214514(q);
    a->w18 &= ~(m << n);
}
