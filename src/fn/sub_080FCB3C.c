#include "global.h"

struct S080FCB3C { u8 filler[0x3d0]; u8 *q; };
void sub_0821FE6C(u8 *);

void sub_080FCB3C(struct S080FCB3C *s)
{
    u8 *p = s->q + 0xd94;
    s32 i = 1;
    do {
        sub_0821FE6C(p);
        p += 0x54;
    } while (--i >= 0);
}
