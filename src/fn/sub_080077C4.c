#include "global.h"

struct B { u8 f[6]; u8 fl; u8 g[0x11]; s32 x; s32 y; };
u8 sub_082212DC(s32, s32, u32, u32);
void sub_080070D0(void *, struct B *);

void sub_080077C4(void *a, struct B *b)
{
    if (sub_082212DC(b->y, b->x, 0x2d, 0x5a))
        b->fl |= 1;
    else
        sub_080070D0(a, b);
    b->x++;
}
