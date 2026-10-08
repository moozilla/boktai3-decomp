#include "global.h"

struct T { u8 filler[0x698]; u16 h698; u8 f2[0x69a - 0x69a]; u16 h69a; };
struct S { u8 filler[0x3d0]; struct T *t; };

void sub_080DED24(struct S *s)
{
    struct T *t = s->t;
    u16 *p = &t->h698;
    if (*p != 0)
        *p = *p - 1;
    p = &t->h69a;
    if (*p != 0)
        *p = *p - 1;
}
