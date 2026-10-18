#include "global.h"
extern u8 *gUnk_020000DC;
s32 TextState_GetNumberWidth(u8 *, s32);
s32 MessageWindow_GetNumberWidth(s32 a)
{
    s32 r;
    if (gUnk_020000DC)
        r = TextState_GetNumberWidth(gUnk_020000DC + 0x18, a);
    else
        r = 0;
    return r;
}
