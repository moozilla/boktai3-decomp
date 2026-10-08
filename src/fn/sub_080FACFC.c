#include "global.h"

struct Q { u8 filler[0xe4c]; u32 f; };
struct S080FACFC {
    u8 filler0[0x13a]; u16 h;
    u8 filler1[0x2d0 - 0x13c]; u32 f;
    u8 filler2[0x3d0 - 0x2d4]; struct Q *q;
};

void sub_080FACFC(struct S080FACFC *s)
{
    struct Q *q = s->q;
    u32 m = 0x100000;
    u32 *p = &s->f;
    u32 *p2;
    u32 m2;
    *p = *p | m;
    p2 = &q->f;
    m2 = ~0x11;
    *p2 = *p2 & m2;
    s->h &= ~4;
}
