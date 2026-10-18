#include "global.h"
struct PaletteFlashTarget {
    u8 unknown0[0x230];
    u16 palette[16];
};
struct PaletteFlash {
    u8 unknown0[0xc];
    const u16 *flashPalette, *normalPalette;
    u8 framesRemaining;
    u8 unknown15;
    u16 paletteId;
    struct PaletteFlashTarget *target;
};
void PaletteFlash_SelectPalette(struct PaletteFlash *flash, s32 mode)
{
    struct PaletteFlashTarget *target = flash->target;
    switch (mode) {
        case 0:
            CpuSet(flash->normalPalette, target->palette, 16);
            flash->framesRemaining = mode;
            break;
        case 1:
            CpuSet(flash->flashPalette, target->palette, 16);
            flash->framesRemaining = 8;
            break;
    }
}
