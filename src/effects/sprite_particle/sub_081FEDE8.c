#include "engine/shared/global.h"
struct A {
    u8 p0[0x18];
    u32 w18;
};
void Sprite_Remove(void *);
void sub_081FEDE8(struct A *a, u32 *b, u32 n)
{
    u32 t;
    u32 *q = (u32 *)((u8 *)b + 0x88);
    t = *q;
    *q = t | 1;
    Sprite_Remove(q);
    a->w18 &= ~(1 << n);
}
