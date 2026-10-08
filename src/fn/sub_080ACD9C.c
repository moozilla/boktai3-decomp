#include "global.h"

struct P { u8 f[0x2dc]; u32 flags; };
void sub_0807E9D0(s32, s32);

void sub_080ACD9C(s32 a, s32 b, struct P *p)
{
    u32 f;
    sub_0807E9D0(a, b);
    f = p->flags;
    if (f & 0x20)
        p->flags = f | 0x80000;
}
