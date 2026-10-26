#include "global.h"
extern u8 *gMessageWindow;
s32 TextState_GetNumberWidth(u8 *, s32);
s32 MessageWindow_GetNumberWidth(s32 a)
{
    s32 r;
    if (gMessageWindow)
        r = TextState_GetNumberWidth(gMessageWindow + 0x18, a);
    else
        r = 0;
    return r;
}
