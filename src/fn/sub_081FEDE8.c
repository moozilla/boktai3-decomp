#include "global.h"
struct A { u8 p0[0x18]; u32 w18; };
void sub_08214514(void *);
void sub_081FEDE8(struct A *a, u32 *b, u32 n)
{
    u32 m, t;
    u32 *q = (u32 *)((u8 *)b + 0x88);
    t = *q; m = 1; *q = t | m;
    sub_08214514(q);
    a->w18 &= ~(m << n);
}
