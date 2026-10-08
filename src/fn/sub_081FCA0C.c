#include "global.h"
struct A { u8 p0[0x18]; u32 w18; };
struct B { u8 p[0x9c]; u32 w9c; u8 p1[0x122 - 0xa0]; u16 h122; };
void sub_08214514(void *);
void sub_081FCA0C(struct A *a, struct B *b, u32 n)
{
    u32 m;
    b->h122 |= 4;
    b->w9c = (b->w9c & ~2) | (m = 1);
    sub_08214514(&b->w9c);
    a->w18 &= ~(m << n);
}
