#include "global.h"

struct S { u8 f[7]; u8 a; u8 g[0x60]; u32 b; };
void sub_08009F54(u32 x, struct S *p)
{
    if (p->a != 0) {
        p->a = 0;
        p->b |= 1;
    }
}
