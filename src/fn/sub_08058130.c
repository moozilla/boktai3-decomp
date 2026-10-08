#include "global.h"

struct S { u8 f[0x230]; u16 a[0x900]; u16 b; };

void sub_08058130(struct S *p, s32 i, u16 v)
{
    p->a[i] = v;
    p->b = 1;
}
