#include "global.h"
s32 sub_0811E6F0(u8 *s, s32 x)
{
    s32 r;
    if (*(u8 *)(s + 0xc) == 0)
        return x;
    r = x + *(u16 *)(s + 6);
    if (r < 0)
        r = 1;
    return r;
}
