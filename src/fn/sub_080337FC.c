#include "global.h"
u8 *sub_08033634(void);
void sub_08033AF8(void);
s32 sub_080337FC(void)
{
    u32 *p = (u32 *)sub_08033634();
    s32 r;
    if (p) {
        p[0x5c / 4] = (u32)sub_08033AF8;
        r = 0;
    } else
        r = -1;
    return r;
}
