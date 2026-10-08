#include "global.h"
struct A { u8 p0[0x18]; u32 w18; u8 p1[0xb1c - 0x1c]; s32 cnt; };
struct B { u8 p[0x2c]; u32 w2c; };
void sub_08214514(void *);
void sub_081FB86C(struct A *a, struct B *b, u32 n)
{
    a->cnt--;
    b->w2c |= 1;
    sub_08214514(&b->w2c);
    a->w18 &= ~(1 << n);
}
