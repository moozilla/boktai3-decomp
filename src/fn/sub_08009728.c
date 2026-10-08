#include "global.h"

struct S { u8 f[0x19]; u8 a; u8 b; };
u32 sub_08009728(struct S *p, u8 v)
{
    p->a = v;
    p->b = 1;
}
