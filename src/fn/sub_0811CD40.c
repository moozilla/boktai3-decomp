#include "global.h"
void sub_0811CD40(u8 *p, u32 m)
{
    u32 *q;
    s32 i;
    m = ~m;
    q = (u32 *)(p + 8);
    i = 4;
    do {
        *q &= m;
        q += 0x18;
    } while (--i >= 0);
}
