#include "global.h"
#include "render.h"

struct Hud {
    u8 pad0[0xc70];
    u8 paletteEnabled;
    u8 padC71[0x77f];
    u8 paletteState;
};
extern const u16 gUnk_08610E78[];
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 PauseMenu_IsEquipmentSelectionEnabled(struct Hud *);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void Hud_LoadConditionalPaletteRows(struct Hud *hud)
{
    const u16 *source = gUnk_08610E78;
    s32 row, nextRow, column;

    for (row = 0; row < 4; row = nextRow) {
        u16 *primary = Hud_GetTilemapCell(0, 0, row + 16);
        u16 *secondary = Hud_GetTilemapCell(0, 22, row + 16);
        u16 *tertiary = Hud_GetTilemapCell(0, 26, row + 16);
        nextRow = row + 1;
        for (column = 3; column >= 0; column--) {
            if (hud->paletteEnabled)
                *primary++ = *source;
            if (!PauseMenu_IsEquipmentSelectionEnabled(hud)) {
                *secondary++ = *source;
                *tertiary++ = *source;
            }
            source++;
        }
    }
    hud->paletteState = 0;
    Palette_UpdateHighlightPulse(gBgPaletteBank13Color2, &hud->paletteState);
}