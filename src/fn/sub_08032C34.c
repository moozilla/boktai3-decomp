#include "global.h"

struct S { u8 f[6]; u8 a; u8 b; u8 g[0xf]; u8 c; u8 h[0x108 - 0x18]; u8 d; };
void sub_08032C34(struct S *p)
{
    u32 z = 0;
    p->a = z;
    p->d = z;
    p->b = z;
    if (p->c != 2)
        p->c = z;
}
