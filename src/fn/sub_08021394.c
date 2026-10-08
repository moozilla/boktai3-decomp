#include "global.h"

struct S { u8 f[0x18]; u8 a[0x40]; u32 b; };
void sub_08020CD4(u8 *, u32, u32);
u32 sub_08021394(struct S *p, u32 x)
{
    sub_08020CD4(p->a, x, 7);
    return p->b = 0;
}
