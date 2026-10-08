#include "global.h"
struct E { u32 fl; u8 pad[0x2c - 4]; };
struct S { u8 f[0x250]; struct E e[6]; };
void sub_081C1C6C(struct S *p)
{
    s32 i;
    u32 m = 1;
    struct E *e = p->e;
    for (i = 5; i >= 0; i--) { e->fl |= m; e++; }
}
