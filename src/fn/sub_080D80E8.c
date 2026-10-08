#include "global.h"

struct T { u8 filler[0x6d0]; u16 h6d0; u8 f2[0x6d6 - 0x6d2]; u16 h6d6; };
struct S { u8 filler[0x2d4]; u16 flags; u8 f2[0x3d0 - 0x2d6]; struct T *t; };

void sub_080D80E8(struct S *s)
{
    struct T *t = s->t;
    u32 m = 0x40;
    if (!(s->flags & m)) {
        u16 *p = &t->h6d0;
        if (*p != 0)
            *p = *p - 1;
        p = &t->h6d6;
        if (*p != 0)
            *p = *p - 1;
    }
}
