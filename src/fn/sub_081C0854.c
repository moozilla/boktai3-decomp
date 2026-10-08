#include "global.h"
struct S { u8 f[0x30]; void *a; u8 g[0x1c]; void (*cb)(void *); u32 n; };
void sub_081C35EC(void *, u32, u32);
void sub_081C0874(void *);
void sub_081C0854(struct S *p)
{
    sub_081C35EC(p->a, 0, 0x5a);
    p->cb = sub_081C0874;
    p->n = 0;
}
