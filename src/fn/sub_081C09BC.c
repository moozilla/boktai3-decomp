#include "global.h"
struct S { u8 f[0x30]; void *a; u8 g[0x1c]; void (*cb)(void *); u32 n; };
void sub_081C3700(void *);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C09F8(void *);
void sub_081C09BC(struct S *p)
{
    sub_081C3700(p->a);
    sub_082279A8(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->cb = sub_081C09F8;
    p->n = 0;
}
