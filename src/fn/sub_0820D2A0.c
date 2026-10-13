#include "global.h"
struct B {
    u8 a[0x7c];
    u32 w7c;
};
struct A {
    u8 a[0x18];
    u32 w18;
};
void Sprite_Remove(u32 *);
void sub_0820D2A0(struct A *a, struct B *b, u32 n)
{
    b->w7c |= 1;
    Sprite_Remove(&b->w7c);
    a->w18 &= ~(1 << n);
}
