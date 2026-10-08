#include "global.h"

struct T { u8 filler[0x698]; u16 h698; u16 h69a; };
struct S { u8 filler[0x1e6]; u16 st; u8 f2[0x3d0 - 0x1e8]; struct T *t; };

s32 sub_080DF298(struct S *s)
{
    struct T *t = s->t;
    u16 v;
    if (s->st == 0 || s->st == 2)
        v = t->h698;
    else {
        if (t->h698 == 0)
            return 1;
        v = t->h69a;
    }
    if (v != 0)
        return 0;
    return 1;
}
