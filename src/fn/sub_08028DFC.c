#include "global.h"

u8 *sub_0802140C(u32);

u8 sub_08028DFC(void)
{
    u8 *p = sub_0802140C(0);
    u8 r;
    if (p)
        r = p[0x384];
    else
        r = 0;
    return r;
}
