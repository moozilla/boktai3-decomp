#include "global.h"
struct E { u32 fl; u8 pad[0x2c - 4]; };
struct S { u8 f[0x100]; struct E e[7]; };
void sub_081C1D0C(struct S *p)
{
    s32 i;
    u32 m = 1;
    struct E *e = p->e;
    for (i = 6; i >= 0; i--) { e->fl |= m; e++; }
}
