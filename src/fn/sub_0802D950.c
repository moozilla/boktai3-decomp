#include "global.h"

struct S { u8 f[0x1c]; u16 a; };
u32 sub_0802D93C(struct S *, u16);
u32 sub_0802D950(struct S *p, u16 v)
{
    p->a = v;
    return sub_0802D93C(p, p->a);
}
