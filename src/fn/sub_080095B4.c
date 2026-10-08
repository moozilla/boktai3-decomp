#include "global.h"

void sub_08215284(void *, void *);
struct A { u8 f0[4]; u8 t; u8 f5[0x4B]; u8 sub[1]; };

u32 sub_080095B4(struct A *a, u8 *b)
{
    u8 *p = a->sub;
    if (a->t == 0)
        sub_08215284(p, b + 0x6b);
}
