#include "global.h"
struct P { u32 a[8]; };
struct S { u8 f[0x18]; struct P p; struct P *q; };
struct P *sub_0821A520(u32, u32);
void sub_082196C4(void *, void *);
void sub_08122D20(struct S *s)
{
    s->q = sub_0821A520(0xcb05, 0x1bc7);
    s->p = *s->q;
    sub_082196C4(&s->p, s->q);
}
