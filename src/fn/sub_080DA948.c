#include "global.h"

struct T { u8 filler[0x6f2]; u16 h; };
struct S { u8 filler[0x2b5]; u8 b; u8 filler2[0x3d0 - 0x2b6]; struct T *t; };

void sub_080DA948(struct S *s)
{
    if (s->b != 0x27)
    {
        u16 *p = &s->t->h;
        u32 m = 0xFFFFEBFF;
        *p = *p & m;
    }
}
