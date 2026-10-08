#include "global.h"

struct S { u8 f[0x40]; u16 a; u8 g[0x56-0x42]; u16 b; };

void sub_080670A0(s32 a, s32 b, struct S *p)
{
    p->a |= 2;
    p->b |= 4;
}
