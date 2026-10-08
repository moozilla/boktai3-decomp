#include "global.h"

struct T { u8 filler[0x696]; u16 h; };
struct S { u8 filler[0x3d0]; struct T *t; };

void sub_080DED54(struct S *s)
{
    u16 *p = &s->t->h;
    if (*p != 0)
        *p = *p - 1;
}
