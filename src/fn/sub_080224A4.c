#include "global.h"

struct S { u8 f[0xaa]; u8 a; u8 g[0x298]; u8 b; };
void sub_080224A4(struct S *p, u32 v)
{
    if (p->b == 0)
        v = 3;
    p->a = v;
}
