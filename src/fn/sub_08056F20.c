#include "global.h"

struct S { u8 f[0x78]; s32 *t; };

s32 sub_08056F20(struct S *p, u32 i)
{
    { s32 *q = p->t; return *(s32 *)((u8 *)q + (i << 2)) * 10; }
}
