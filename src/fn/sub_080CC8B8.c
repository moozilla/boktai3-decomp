#include "global.h"

void sub_082260A4(u32);
void sub_080C4E8C(void *);
void sub_080C52BC(void *);
void sub_080C53A8(void *);

struct T { u8 filler[0x6ac]; u16 h6ac; u16 h6ae; };
struct S { u8 filler[0x2d4]; u16 flags; u8 f2[0x3d0 - 0x2d6]; struct T *t; };

s32 sub_080CC8B8(struct S *s)
{
    struct T *t = s->t;
    u32 m = 0x40;
    if (!(s->flags & m)) {
        u16 *p = &t->h6ae;
        if (*p != 0) {
            sub_082260A4(t->h6ac);
            *p = *p - 1;
        }
    }
    sub_080C4E8C(s);
    sub_080C52BC(s);
    sub_080C53A8(s);
    return 1;
}
