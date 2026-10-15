// MATCHING_CONCESSION: register-pinning
#include "scene_hud_map.h"
#define SceneHud_DrawDecimal sub_0805DD98
void SceneHud_DrawDecimal(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    u16 *cell;
    s32 base;
    {
        register s32 initial asm("r5") = value;
        asm("" : "+r"(initial));
        value = initial;
    }
    cell = sub_0805DD18(0, x, y);
    base = (style << 4) + 0x21;
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
