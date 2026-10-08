#include "global.h"

struct Q { u8 filler[0xec8]; u8 v[3]; u8 idx; };
struct S080FB214 { u8 filler[0x3d0]; struct Q *q; };

void sub_080FB214(struct S080FB214 *s, u32 x)
{
    struct Q *q = s->q;
    q->v[q->idx] = x;
    q->idx++;
    if (q->idx > 2)
        q->idx = 0;
}
