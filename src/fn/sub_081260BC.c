#include "global.h"
struct S { u8 f[0x18]; u32 p; u8 g[0x69c - 0x1c]; u32 c; };
extern struct S *gUnk_02000150;
u32 sub_08215184(u32);
s32 sub_081260BC(struct S *s)
{
    s->p = sub_08215184(0x1c1e);
    s->c = 0;
    gUnk_02000150 = s;
    return 0;
}
