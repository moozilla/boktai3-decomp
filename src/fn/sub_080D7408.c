#include "global.h"

struct T { u8 filler[0x6d0]; u16 h6d0; u8 f2[0x6d8 - 0x6d2]; u32 v6d8; };
struct S { u8 filler[0xf0]; u32 v; u8 f2[0x3d0 - 0xf4]; struct T *t; };

s32 sub_080D7408(struct S *s)
{
    struct T *t = s->t;
    if (s->v >= t->v6d8 && t->h6d0 != 0)
        return 0;
    return 1;
}
