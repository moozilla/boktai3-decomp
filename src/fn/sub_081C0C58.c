#include "global.h"
struct E { u32 fl; u8 pad[0x5c]; };
struct S { u8 f[0x98]; struct E e[8]; };
void sub_081C0C58(struct S *p)
{
    s32 i;
    u32 m = 1;
    struct E *e = p->e;
    for (i = 7; i >= 0; i--) { e->fl |= m; e++; }
}
