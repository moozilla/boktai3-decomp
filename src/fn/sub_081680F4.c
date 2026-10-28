// MATCHING_CONCESSION: register-pinning
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
void Palette_UpdateHighlightPulse(u16 *);
void Hud_LoadConditionalPaletteRows(struct Hud *input)
{
    struct Hud *hud = input;
    register s32 i asm("r8");
    const u16 *source = gUnk_08610E78;
    i = 0;
    do {
        u16 *primary, *secondary, *tertiary;
        s32 row = i + 16;
        s32 j;
        primary = Hud_GetTilemapCell(0, 0, row);
        secondary = Hud_GetTilemapCell(0, 22, row);
        tertiary = Hud_GetTilemapCell(0, 26, row);
        i++;
        j = 3;
        do {
            if (hud->paletteEnabled)
                *primary++ = *source;
            if (!PauseMenu_IsEquipmentSelectionEnabled(hud)) {
                *secondary++ = *source;
                *tertiary++ = *source;
            }
            source++;
        } while (--j >= 0);
    } while (i <= 3);
    {
        u8 *state = &hud->paletteState;
        register u32 zero asm("r0") = 0;
        *state = zero;
    }
    Palette_UpdateHighlightPulse(gBgPaletteBank13Color2);
}
