#include "global.h"

extern u8 *gUnk_02000710;
s32 Div(s32, s32);

s32 sub_0813E208(void *unused, s32 n)
{
    s32 a = Div(n, 10);
    s32 b = Div(n, 2);
    s16 v1 = *(s16 *)(gUnk_02000710 + 0x2A);
    s32 c = Div(v1 - *(s16 *)(gUnk_02000710 + 0x28), v1);
    return n + a - b * c;
}
