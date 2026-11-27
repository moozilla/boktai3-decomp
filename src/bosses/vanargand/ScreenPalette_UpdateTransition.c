#include "global.h"
#include "engine/video/render.h"
#include "gba/defines.h"
#include "bosses/vanargand/screen_palette_transition_layout.h"

void Blend_SetAlpha(u32, s32, s32);
void Palette_InterpolateBanks(void *, void *, void *, int, int);

void ScreenPalette_UpdateTransition(struct ScreenPaletteTransition *s)
{
    s32 sourceBank;
    u8 *palettes = s->palettes;
    if (s->timer) {
        if (s->state == 1 && s->timer > 64)
            Blend_SetAlpha(1, (96 - s->timer) / 2, 16);
        else
            Blend_SetAlpha(1, 16, 16);
    }
    if (s->timer <= 32) {
        sourceBank = s->state * 3 + 11;
        Palette_InterpolateBanks(gBgPaletteBank9, palettes + sourceBank * PLTT_SIZE_4BPP,
                                 palettes + (s->state * 3 + 10) * PLTT_SIZE_4BPP, s->timer, 5);
    } else if (s->timer <= 64) {
        sourceBank = s->state * 3 + 10;
        Palette_InterpolateBanks(gBgPaletteBank9, palettes + sourceBank * PLTT_SIZE_4BPP,
                                 palettes + (s->state * 3 + 9) * PLTT_SIZE_4BPP, s->timer - 32, 5);
    } else {
        sourceBank = s->state * 3 + 9;
        CpuSet(s->palettes + (sourceBank * PLTT_SIZE_4BPP), gBgPaletteBank9, 0x04000008);
    }
}
