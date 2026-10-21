#include "global.h"
#define SceneHud_DrawStyledDecimal PanelText_DrawDigits
#define SceneHud_GetLayerMapCell   Video_GetWrappedBackgroundMapEntry
s32 Div(s32, s32);
u16 *Video_GetWrappedBackgroundMapEntry(s32, u8 *, u32);
void PanelText_DrawDigits(s32 value, s32 divisor, u8 *x, u32 y, s32 style)
{
    u16 *cell;
    s32 base = 0x21;
    if (!style)
        base = 0x11;
    cell = Video_GetWrappedBackgroundMapEntry(3, x, y);
    if (divisor > 0) {
        do {
            s32 digit = Div(value, divisor);
            *cell++ = digit + base;
            do {
                value -= digit * divisor;
            } while (0);
            divisor = Div(divisor, 10);
        } while (divisor > 0);
    }
}
