#include "global.h"
struct E { u32 fl; u8 pad[0x34]; };
struct S { u8 p0[0xfa]; u16 hfa; u8 p1[4]; struct E e[12]; };
void sub_081FDB9C(struct S *p)
{
    s32 i;
    u32 m = 1;
    struct E *e = p->e;
    for (i = 11; i >= 0; i--) { e->fl |= m; e++; }
    p->hfa = 1;
}
