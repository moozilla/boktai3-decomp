#include "global.h"
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void SceneHud_DrawDecimalDigits(s32 value, s32 div, s32 x, s32 y, s32 flag)
{
    u32 base = 0x31;
    u16 *cell;
    if (!flag) {
        base = 0x21;
    }
    cell = Hud_GetTilemapCell(0, x, y);
    while (div > 0) {
        s32 d = Div(value, div);
        u16 t = d + base;
        *cell++ = t | 0xF000;
        do {
            value -= div * d;
        } while (0);
        div = Div(div, 10);
    }
}
