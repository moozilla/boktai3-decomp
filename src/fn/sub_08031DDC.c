#include "global.h"
extern u8 *gMessageWindow;
s32 TextState_MeasureStringLine(u8 *, s32);
s32 MessageWindow_GetStringWidth(s32 a)
{
    s32 r;
    if (gMessageWindow)
        r = TextState_MeasureStringLine(gMessageWindow + 0x18, a);
    else
        r = 0;
    return r;
}
