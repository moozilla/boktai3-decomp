#include "global.h"

extern u8 *gUnk_020000DC;
u32 TextState_MeasureTextColumns(u8 *);
u32 MessageWindow_GetRemainingTextWidth(void)
{
    u8 *p = gUnk_020000DC;
    if (p == 0)
        return 0;
    return TextState_MeasureTextColumns(p + 0x18);
}
