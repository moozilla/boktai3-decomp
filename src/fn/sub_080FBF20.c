#include "global.h"

struct S080FBF20 { u8 filler[0x3d0]; u8 *q; };
void sub_08214514(u8 *);

void sub_080FBF20(struct S080FBF20 *s)
{
    u8 *p = s->q + 0x708;
    s32 i = 3;
    do {
        if (p[4])
            sub_08214514(p);
        p += 0x60;
    } while (--i >= 0);
}
