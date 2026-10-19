#define gRegistry3F74TransitionPalette gUnk_03004FC0
#include "global.h"

struct PaletteTransition {
    u8 unknown[0x1C];
    u16 *palettes;
};
extern u16 gUnk_03004FC0[];
void Palette_InterpolateBanks(void *, void *, void *, int, int);

void Registry3F74_ApplyPaletteStep(struct PaletteTransition *transition, u32 step)
{
    u16 *palettes = transition->palettes;
    switch (step) {
        case 0:
            Palette_InterpolateBanks(gUnk_03004FC0, palettes, palettes + 32, 48, 6);
            break;
        case 1:
            Palette_InterpolateBanks(gUnk_03004FC0, palettes, palettes + 32, 16, 6);
            break;
        case 2:
            CpuSet(palettes, gUnk_03004FC0, 0x04000008);
            break;
        case 3:
            Palette_InterpolateBanks(gUnk_03004FC0, palettes, palettes + 16, 16, 6);
            break;
        case 4:
            Palette_InterpolateBanks(gUnk_03004FC0, palettes + 16, palettes + 32, 48, 6);
            break;
        case 5:
            CpuSet(palettes + 32, gUnk_03004FC0, 0x04000008);
            break;
    }
}
