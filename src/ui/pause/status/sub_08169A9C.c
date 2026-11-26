#include "global.h"

#define SceneHud_DrawThreeDigitValue StatusPanel_DrawDecimalValue
#define SceneHud_GetTextDestination  Hud_GetTilemapCell
#define SceneHud_MapDigitGlyph       Hud_GetStyleTileIndex
s32 Div(s32, s32);
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Hud_GetStyleTileIndex(s32, s32);
void StatusPanel_DrawDecimalValue(void *owner, s32 x, s32 y, s32 value)
{
    s32 digits[3];
    s32 started;
    u16 *dest;
    s32 i;
    digits[0] = Div(value, 100);
    value -= digits[0] * 100;
    digits[1] = Div(value, 10);
    value -= digits[1] * 10;
    digits[2] = value;
    started = 0;
    dest = Hud_GetTilemapCell(0, x, y);
    i = 0;
    do {
        if (digits[i] > 0) {
            started = 1;
            *dest = Hud_GetStyleTileIndex(digits[i], 0) | (s16)0xD000;
        } else if (started || i == 2) {
            *dest = Hud_GetStyleTileIndex(digits[i], 0) | (s16)0xD000;
        } else {
            *dest = 0xD000;
        }
        dest++;
        i++;
    } while (i <= 2);
    *dest = 0xD07B;
}
