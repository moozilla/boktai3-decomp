#include "global.h"

extern u8 *gUnk_02000710;
s32 Div(s32, s32);

s32 sub_0813E1D8(void *unused, s32 n)
{
    s32 a = Div(n, 4);
    s16 v1 = *(s16 *)(gUnk_02000710 + 0x2A);
    s32 b = Div(v1 - *(s16 *)(gUnk_02000710 + 0x28), v1);
    return a + n * b;
}
