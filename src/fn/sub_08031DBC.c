#include "global.h"
#include "ram.h"
s32 TextState_GetNumberWidth(u8 *, s32);
s32 MessageWindow_GetNumberWidth(s32 a)
{
    s32 r;
    if (((u8 *)gMessageWindow))
        r = TextState_GetNumberWidth(((u8 *)gMessageWindow) + 0x18, a);
    else
        r = 0;
    return r;
}
