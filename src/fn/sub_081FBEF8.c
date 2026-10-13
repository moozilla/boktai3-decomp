#include "global.h"
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0xf3c - 0x1c];
    s32 cnt;
};
struct B {
    u8 p[0x30];
    u32 w30;
};
void Sprite_Remove(void *);
void sub_081FBEF8(struct A *a, struct B *b, u32 n)
{
    a->cnt--;
    b->w30 |= 1;
    Sprite_Remove(&b->w30);
    a->w18 &= ~(1 << n);
}
