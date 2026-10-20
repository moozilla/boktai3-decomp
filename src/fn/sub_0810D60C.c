#include "global.h"
// MATCHING_CONCESSION: register-pinning

#define PanelText_GetMapCell Bg_GetWrappedTilemapEntry
s32 Div(s32, s32);
u8 *Bg_GetWrappedTilemapEntry(u32, u32, u32);
void PanelText_DrawDecimal(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    s32 base = 0x21;
    register u16 *cell asm("r6");
    if (style == 0)
        base = 0x11;
    cell = (u16 *)Bg_GetWrappedTilemapEntry(3, x, y);
    while (divisor > 0) {
        s32 digit = Div(value, divisor);
        *cell = digit + base;
        cell++;
        value = value - digit * divisor;
        divisor = Div(divisor, 10);
    }
}
