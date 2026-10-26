#include "global.h"
#include "render.h"

struct PaletteTransition {
    u8 unknown[0x1C];
    u16 *palettes;
};
void Palette_InterpolateBanks(void *, void *, void *, int, int);

void Registry3F74_ApplyPaletteStep(struct PaletteTransition *transition, u32 step)
{
    u16 *palettes = transition->palettes;
    switch (step) {
        case 0:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 32, 48, 6);
            break;
        case 1:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 32, 16, 6);
            break;
        case 2:
            CpuSet(palettes, gBgPaletteBuffer, 0x04000008);
            break;
        case 3:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 16, 16, 6);
            break;
        case 4:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes + 16, palettes + 32, 48, 6);
            break;
        case 5:
            CpuSet(palettes + 32, gBgPaletteBuffer, 0x04000008);
            break;
    }
}
