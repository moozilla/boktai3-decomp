#include "global.h"

struct S { u8 f[0x22e]; u16 a; u8 g[0x250-0x230]; u32 b; };

void sub_08058150(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
