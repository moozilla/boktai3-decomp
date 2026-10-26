#include "global.h"
#include "ram.h"

u32 TextState_SetString(u8 *, u32, u32);
s32 ScrollingDialogue_SetStringArgument(u32 a, u32 b)
{
    u8 *p = gScrollingDialogue;
    s32 r;
    if (p == 0)
        r = -1;
    else
        r = TextState_SetString(p + 0x18, a, b);
    return r;
}
