#include "global.h"

extern u8 *gUnk_02000710;
u8 *sub_08227F5C(u8);
s32 Div(s32, s32);

s32 sub_0813E244(u8 *p, s32 n)
{
    u8 *t = sub_08227F5C(p[0xB0]);
    s32 a = Div(Div(n, 2), 100 - t[2]);
    s32 b = *(s16 *)(gUnk_02000710 + 0x40) - t[2];

    return n - b * a;
}
