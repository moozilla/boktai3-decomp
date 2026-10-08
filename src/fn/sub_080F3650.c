#include "global.h"

struct A { u16 a; u8 f[0x1a]; };
struct B { u32 b; u8 f[0x18]; };

void sub_080F3650(u8 *p, u32 b, s32 a)
{
    struct B *y = (struct B *)(p + 0x164);
    struct A *x = (struct A *)(p + 0x15e);
    s32 i = 3;
    do {
        x->a = a;
        y->b = b;
        y++;
        x++;
    } while (--i >= 0);
}
