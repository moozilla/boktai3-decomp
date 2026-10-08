#include "global.h"

struct S { u8 f[0x18]; u32 a; };
u32 sub_08045D04(struct S *p, u32 v)
{
    if (p == 0) return 0;
    p->a = v;
}
