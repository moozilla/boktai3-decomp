#include "global.h"
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x9e8 - 0x1c]; s32 cnt; };
struct B { u8 p[0x80]; u8 h[6]; u16 h86; u8 q[0x298 - 0x88]; u8 e298[0x340 - 0x298]; u32 w33c; u8 r[0x1b8 - 0x1b8]; };
void sub_08013728(void *);
void sub_08160624(u32);
struct H { u8 f[6]; u16 h; };
struct W { u8 f[8]; u32 w; };
void sub_08200C40(struct A *a, u8 *b, u32 n)
{
    struct H *s;
    struct W *w;
    u32 m;
    a->cnt--;
    sub_08013728(b + 0x298);
    sub_08160624(*(u32 *)(b + 0x33c));
    s = (struct H *)(b + 0x80);
    s->h = 4 | s->h;
    w = (struct W *)(b + 0x1b8);
    w->w = w->w | 1;
    a->w18 &= ~(1 << n);
}
