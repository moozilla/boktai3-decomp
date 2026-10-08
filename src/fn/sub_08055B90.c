#include "global.h"

struct S { u8 f[0x1ba]; u16 a; u8 g[0x1d0-0x1bc]; u32 b; };

void sub_08055B90(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
