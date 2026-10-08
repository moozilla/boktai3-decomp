#include "global.h"
struct E { u32 fl; u8 pad[0x60 - 4]; };
struct S { u8 f[0x3a0]; struct E e[5]; };
void sub_081C1A58(struct S *p)
{
    s32 i;
    u32 m = ~1;
    struct E *e = p->e;
    for (i = 4; i >= 0; i--) { e->fl &= m; e++; }
}
