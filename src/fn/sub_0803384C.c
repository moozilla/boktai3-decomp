#include "global.h"
u8 *sub_08033634(void);
void sub_08033AF0(void);
s32 sub_0803384C(void)
{
    u32 *p = (u32 *)sub_08033634();
    s32 r;
    if (p) {
        p[0x5c / 4] = (u32)sub_08033AF0;
        r = 0;
    } else
        r = -1;
    return r;
}
