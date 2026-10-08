#include "global.h"

struct S { u8 f[0x18]; u32 a; u32 b; };
void sub_0821A284(u32, struct S *, u32);
u32 sub_08215184(u32);
u32 sub_0803F6C4(struct S *p)
{
    sub_0821A284(0xF0DE, p, 0);
    p->b = 0;
    p->a = sub_08215184(0x1C1A);
    return 0;
}
