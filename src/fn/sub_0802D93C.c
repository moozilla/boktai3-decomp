#include "global.h"

struct S { u8 f[0x1e]; u16 a; u8 g[0xd4 - 0x20]; u8 b[1]; };
u32 sub_08042798(u8 *, u32);
u32 sub_0802D93C(struct S *p, u32 v)
{
    u8 *q = p->b;
    p->a = v;
    return sub_08042798(q, p->a);
}
