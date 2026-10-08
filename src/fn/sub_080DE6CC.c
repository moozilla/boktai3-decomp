#include "global.h"

struct S { u8 filler[0x2d0]; u32 flags; };

void sub_080DE6CC(struct S *s)
{
    u32 *p = &s->flags;
    u32 m = ~2;
    *p = *p & m;
}
