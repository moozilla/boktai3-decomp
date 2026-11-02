#include "scene_hud_map.h"
static inline void DrawDecimalDigits(u16 *cell, s32 value, s32 divisor, s32 style)
{
    s32 base = (style << 4) + 0x21;
    if (divisor > 0) {
        do {
            s32 digit = Div(value, divisor);
            u32 glyph = (u16)(digit + base);
            *cell++ = glyph | 0xF000;
            value -= digit * divisor;
            divisor = Div(divisor, 10);
        } while (divisor > 0);
    }
}
void SceneHud_DrawDecimal(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    u16 *cell = SceneHud_GetMapCell(0, x, y);
    DrawDecimalDigits(cell, value, divisor, style);
}
