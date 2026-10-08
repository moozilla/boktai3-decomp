#include "global.h"
struct S { u8 f[0x588]; u16 v; u8 g[0x58a - 0x58a]; u16 a; u16 b; u16 c; };
u32 sub_081C1AD0(struct S *p)
{
    if (p->v > p->c) return 10;
    if (p->v > p->b) return 5;
    if (p->v > p->a) return 2;
    return 1;
}
