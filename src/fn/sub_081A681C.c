// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

struct ScreenPaletteTransition {
    u8 unknown0[4];
    u8 *palettes;
    u8 unknown8[12];
    u8 variant;
    u8 unknown15;
    u16 phase;
};
void Blend_SetAlpha(u32, s32, s32);
void Palette_InterpolateBanks(void *, void *, void *, int, int);

void ScreenPalette_UpdateTransition(struct ScreenPaletteTransition *s)
{
    u8 *palettes = s->palettes;
    if (s->phase) {
        if (s->variant == 1 && s->phase > 64)
            Blend_SetAlpha(1, (96 - s->phase) / 2, 16);
        else
            Blend_SetAlpha(1, 16, 16);
    }
    if (s->phase <= 32) {
        s32 a = s->variant * 3 + 11;
        Palette_InterpolateBanks(gBgPaletteBank9, palettes + a * 32,
                                 palettes + (s->variant * 3 + 10) * 32, s->phase, 5);
    } else if (s->phase <= 64) {
        s32 a = s->variant * 3 + 10;
        Palette_InterpolateBanks(gBgPaletteBank9, palettes + a * 32,
                                 palettes + (s->variant * 3 + 9) * 32, s->phase - 32, 5);
    } else {
        u32 v = s->variant;
        register u32 index asm("r0") = v * 3;
        u32 bank = index + 9;
        CpuSet(s->palettes + (bank << 5), gBgPaletteBank9, 0x04000008);
    }
}
