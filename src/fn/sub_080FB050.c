#include "global.h"

struct Q { u8 filler[0xeb8]; u16 v[4]; };
struct S080FB050 { u8 filler[0x3d0]; struct Q *q; };

void sub_080FB050(struct S080FB050 *s)
{
    u16 *p = s->q->v;
    s32 i = 3;
    do {
        if (*p)
            (*p)--;
        p++;
    } while (--i >= 0);
}
