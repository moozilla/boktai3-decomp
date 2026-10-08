#include "global.h"
struct D { u8 cnt; u8 p; u16 off; };
struct A { u8 p[0x10]; u8 *f10; };
struct C { u8 f0; u8 f1; u8 f2; u8 f3; struct D *f4; u8 *f8; };
extern struct A *gUnk_030052F4;
u32 sub_0821E618(struct C *c)
{
    u8 *base;
    c->f2++;
    if (c->f2 >= c->f4->cnt) c->f2 = 0;
    c->f3 = 0;
    base = gUnk_030052F4->f10;
    c->f8 = base + c->f4->off + (c->f2 << 3);
    return 1;
}
