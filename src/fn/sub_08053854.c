#include "global.h"

struct S { u8 f[0x110]; u16 a; u8 g[0x114-0x112]; u32 b; };

void sub_08053854(struct S *p, u32 v)
{
    p->b = v;
    p->a = 0;
}
