#include "global.h"

struct S { u8 filler[0x876]; s16 v; };
extern struct S *gUnk_02000710;
s32 Div(s32, s32);

s32 sub_0804E008(s32 x)
{
    s32 r = Div(x * gUnk_02000710->v, 10);
    if (r <= 0)
        r = 1;
    return r;
}
