#include "global.h"

struct S { u8 f[0xc4]; u16 a; u32 b; };

void sub_08053DE8(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
