#include "global.h"

struct S { u8 f[5]; u8 a; u8 b; };
u32 sub_08009500(struct S *p, u8 v)
{
    p->a = v;
    p->b = 1;
}
