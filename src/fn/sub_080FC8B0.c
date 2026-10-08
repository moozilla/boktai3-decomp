#include "global.h"

struct S080FC8B0 { u8 filler[0x3d0]; u8 *q; };

void sub_080FC8B0(struct S080FC8B0 *s, u32 a, u32 b)
{
    u8 *q = s->q;
    *(u16 *)(q + 0xa6a) = b;
    *(u32 *)(q + 0xa70) = a;
}
