#include "global.h"
struct S { u8 f[0x30]; void *a; u8 g[0x20]; u32 n; };
s32 sub_081C3010(void *);
void sub_081C0894(void *);
void sub_081C0874(struct S *p)
{
    p->n = p->n + 1;
    if (sub_081C3010(p->a) != 0) sub_081C0894(p);
}
