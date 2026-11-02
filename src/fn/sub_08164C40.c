#include "global.h"
#include "render.h"
#include "game_state.h"

// confidence=low: gauge source is the save-state counter at 0x618.
extern u16 gUnk_0300517A;
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Div(s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32);
struct StoredEnergyGauge {
    u8 padding[0xa88];
    s16 stored;
    u16 timer;
    u16 flash;
};
void Hud_DrawStoredEnergyGauge(struct StoredEnergyGauge *hud, s32 mode)
{
    s32 background, foreground, full, partial, n, amount;
    u16 *cell;
    int paletteChanged;
    s32 stored;
    struct GameState *g;
    if (mode == 1) {
        background = 0x1f;
        if (hud->stored < gGameState->unk618) {
            u16 *timer = &hud->timer;
            s32 bit;
            ++*timer;
            bit = (*timer >> 2) & 1;
            foreground = 0x7fff;
            if (bit)
                foreground = 0x3e0;
        } else if (hud->flash) {
            foreground = 0x7fff;
            --hud->flash;
        } else
            foreground = 0x3e0;
    } else {
        background = 0x39ce;
        if (hud->flash) {
            amount = 0x7fff;
            foreground = amount;
            --hud->flash;
        } else
            foreground = 0x1f;
    }
    paletteChanged = gBgPaletteBank13Color14 != background;
    if (paletteChanged)
        gBgPaletteBank13Color14 = background;
    if ((&gBgPaletteBank13Color14)[-1] != foreground)
        (&gBgPaletteBank13Color14)[-1] = foreground;
    g = gGameState;
    stored = g->unk618;
    hud->stored = stored;
    amount = g->unk618;
    if (amount <= 0) {
        full = 0;
        partial = 0;
    } else if (g->unk61C > 1) {
        s32 scale = 79;
        partial = Div((amount - 1) * scale, g->unk61C - 1) + 1;
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
