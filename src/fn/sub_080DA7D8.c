#include "global.h"

struct T { u8 filler[0x6ea]; u8 cnt; };
struct S { u8 filler[0x2d0]; u32 flags; u8 filler2[0x3d0 - 0x2d4]; struct T *t; };

void sub_080DA7D8(struct S *s)
{
    u8 *p = &s->t->cnt;
    if (*p != 0)
        *p = *p - 1;
    else
    {
        u32 *q = &s->flags;
        u32 m = 0xFEFFFFFF;
        *q = *q & m;
    }
}
