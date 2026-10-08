#include "global.h"
void sub_0813B134(u8 *, u32);
void sub_08145220(u8 *p)
{
    u32 m;
    if (p[0x4f2] != 0)
        sub_0813B134(p, 0x1c);
    else
        sub_0813B134(p, 0x1d);
    m = 0x1100;
    *(u32 *)(p + 0x20) = *(u32 *)(p + 0x20) | m;
}
