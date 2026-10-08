#include "global.h"
s32 Div(s32, s32);
s32 sub_0811E684(u8 *s, s32 x)
{
    s32 r;
    if (*(u8 *)(s + 0xc) == 0)
        return x;
    r = Div(*(u16 *)(s + 6) * x, 10);
    if (r < 0)
        r = 1;
    return r;
}
