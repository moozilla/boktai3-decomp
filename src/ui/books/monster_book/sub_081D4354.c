#include "global.h"
void PaletteTransition_BlendBankColors(u8 *, u32, u32);
void BestiaryDisplay_UpdatePalettePulse(u8 *p)
{
    u32 v = *(u32 *)(p + 0x20);
    if (v > 0x1f)
        v = 0x40 - v;
    PaletteTransition_BlendBankColors(p, v, 5);
    *(u32 *)(p + 0x20) = (*(u32 *)(p + 0x20) + 1) & 0x3f;
}
