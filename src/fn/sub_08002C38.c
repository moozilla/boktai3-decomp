#include "global.h"

void sub_08227950(u16 *, u16 *);
struct S { u8 f[0x27c]; u32 t[1]; };
struct D { u32 a; u32 b; u32 c; };

void sub_08002C38(struct S *p, struct D *d)
{
    u16 x[2];
    u16 *y = &x[1];
    sub_08227950(x, y);
    d->b = p->t[x[0]];
    d->c = p->t[*y];
}
