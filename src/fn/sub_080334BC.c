#include "global.h"

extern u8 *gUnk_020000E0;
u32 TextState_SetString(u8 *, u32, u32);
s32 ScrollingDialogue_SetStringArgument(u32 a, u32 b)
{
    u8 *p = gUnk_020000E0;
    s32 r;
    if (p == 0)
        r = -1;
    else
        r = TextState_SetString(p + 0x18, a, b);
    return r;
}
