#include "global.h"
#include "render.h"

// MATCHING_CONCESSION: register-pinning
// confidence=low: gauge source is the save-state counter at 0x618.
extern u8 *gGameState;
extern u16 gUnk_0300517A;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32);
void Hud_DrawStoredEnergyGauge(u8 *p, s32 mode)
{
    s32 background, foreground, full, partial, n, amount;
    u16 *cell;
    int new_var;
    s32 stored;
    u8 *g;
    if (mode == 1) {
        background = 0x1f;
        if (*(s16 *)(p + 0xa88) < *(s32 *)(gGameState + 0x618)) {
            u16 *timer = (u16 *)(p + 0xa8a);
            s32 bit;
            ++*timer;
            bit = (*timer >> 2) & 1;
            foreground = 0x7fff;
            if (bit)
                foreground = 0x3e0;
        } else if (*(u16 *)(p + 0xa8c)) {
            foreground = 0x7fff;
            --*(u16 *)(p + 0xa8c);
        } else
            foreground = 0x3e0;
    } else {
        background = 0x39ce;
        if (*(u16 *)(p + 0xa8c)) {
            amount = 0x7fff;
            foreground = amount;
            --*(u16 *)(p + 0xa8c);
        } else
            foreground = 0x1f;
    }
    new_var = gBgPaletteBank13Color14 != background;
    if (new_var)
        gBgPaletteBank13Color14 = background;
    if ((&gBgPaletteBank13Color14)[-1] != foreground)
        (&gBgPaletteBank13Color14)[-1] = foreground;
    g = gGameState;
    cell = (u16 *)&*(s32 *)(g + 0x618);
    stored = *(s32 *)cell;
    *(u16 *)(p + 0xa88) = stored;
    asm("" ::: "memory");
    amount = *(s32 *)(g + 0x618);
    if (amount <= 0) {
        full = 0;
        partial = 0;
    } else if (*(s32 *)(g + 0x61c) > 1) {
        s32 scale = 79;
        partial = Div((amount - 1) * scale, *(s32 *)(g + 0x61c) - 1) + 1;
        full = partial >> 3;
        partial -= full << 3;
    } else {
        full = 10;
        partial = 0;
    }
    cell = Hud_GetTilemapCell(0, 29, 1);
    *cell-- = 0xd08c;
    *cell-- = 0xd08b;
    *cell-- = 0xd08a;
    for (n = 0; n < full; n++)
        *cell-- = 0xd089;
    if (partial) {
        *cell-- = (partial + 0x81) | -0x3000;
        n++;
    }
    for (; n < 10; n++)
        *cell-- = 0xd04b;
    *cell = 0xd44c;
    StatDisplay_DrawTwoDigitCounter(-1);
}
