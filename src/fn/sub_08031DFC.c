#include "global.h"
#include "ram.h"

u32 TextState_MeasureTextColumns(u8 *);
u32 MessageWindow_GetRemainingTextWidth(void)
{
    u8 *p = gMessageWindow;
    if (p == 0)
        return 0;
    return TextState_MeasureTextColumns(p + 0x18);
}
