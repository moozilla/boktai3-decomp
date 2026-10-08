#include "global.h"

s32 sub_08074600(s32, s32);

struct S { u8 filler[0x98]; u8 b; u8 f2[0x308 - 0x99]; s32 v; };

void sub_080E7D80(struct S *s)
{
    s->v = sub_08074600(s->b, 0);
}
