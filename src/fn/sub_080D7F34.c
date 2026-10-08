#include "global.h"

s32 sub_08074600(s32, s32);

struct S { u8 filler[0x308]; s32 v; };

void sub_080D7F34(struct S *s)
{
    s->v = sub_08074600(1, 0);
}
