#include "global.h"

struct S { u8 f[0x1AEE]; u16 a; u8 g[0x1CFC-0x1AF0]; s32 b; };

void sub_0806130C(struct S *p, s32 v)
{
    p->b = v;
    p->a = 0xFFFF;
}
