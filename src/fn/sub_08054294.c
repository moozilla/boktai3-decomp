#include "global.h"

struct S { u8 f[0xf4]; u16 a; u8 g[0x110-0xf6]; u32 b; };

void sub_08054294(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
