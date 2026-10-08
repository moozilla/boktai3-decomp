#include "global.h"
struct S { u8 f[0x2c]; u32 lim; u32 a; u32 cnt; };
void sub_081C0538(void *);
void sub_081C0564(void *);
void sub_081C0588(struct S *p)
{
    sub_081C0538(p);
    p->cnt = p->cnt + 1;
    if (p->cnt >= p->lim) sub_081C0564(p);
}
