#include "global.h"

struct T { u8 filler[0x6d2]; u16 h6d2; };
struct S { u8 filler[0x2d4]; u16 flags; u8 f2[0x3d0 - 0x2d6]; struct T *t; };

void sub_080D9AEC(struct S *s)
{
    struct T *t = s->t;
    u32 m = 0x40;
    if (!(s->flags & m)) {
        u16 *p = &t->h6d2;
        if (*p != 0)
            *p = *p - 1;
    }
}
