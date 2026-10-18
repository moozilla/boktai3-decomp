#include "global.h"
extern u8 *gUnk_020000DC;
s32 TextState_MeasureStringLine(u8 *, s32);
s32 MessageWindow_GetStringWidth(s32 a)
{
    s32 r;
    if (gUnk_020000DC)
        r = TextState_MeasureStringLine(gUnk_020000DC + 0x18, a);
    else
        r = 0;
    return r;
}
