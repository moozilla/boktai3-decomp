// MATCHING_CONCESSION: register-pinning
#include "global.h"

// foundry draft (worker best 97.3%); what differs is mostly register allocation and instruction order
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
    s32 mask;
    s32 *digit;
    digits[0] = Div(value, 100);
    value -= digits[0] * 100;
    digits[1] = Div(value, 10);
    value -= digits[1] * 10;
    digits[2] = value;
    started = 0;
    dest = Hud_GetTilemapCell(0, x, y);
    i = 0;
    {
        register s32 t asm("r1") = 0xFFFFD000;
        asm("" : "+r"(t));
        mask = t;
    }
    asm("" : "+r"(mask));
    digit = digits;
    do {
        if (*digit > 0) {
            started = 1;
            *dest = Hud_GetStyleTileIndex(*digit, 0) | mask;
        } else if (started || i == 2) {
            *dest = Hud_GetStyleTileIndex(*digit, 0) | mask;
        } else {
            *dest = 0xD000;
        }
        dest++;
        digit++;
        i++;
    } while (i <= 2);
    *dest = 0xD07B;
}
