#include "global.h"
struct S { u8 f[0x418]; u8 v; };
struct P { u8 f[0xA40]; struct S *s; };
extern u32 gUnk_030053F4;
s32 sub_081680B8(struct P *p)
{
    if (gUnk_030053F4 & 0x1000)
        goto zero;
    if ((u8)(p->s->v - 1) <= 1)
        goto one;
zero:
    return 0;
one:
    return 1;
}
