#include "global.h"
struct B {
    u8 a[0x10];
    u32 w10;
};
struct A {
    u8 a[0x18];
    u32 w18;
};
void Sprite_Remove(u32 *);
u32 sub_082094D4(struct A *a, struct B *b, u32 n)
{
    b->w10 |= 1;
    Sprite_Remove(&b->w10);
    a->w18 &= ~(1 << n);
}
