#include "global.h"
struct BestiaryPulseDisplay {
    u8 pad0[0x20];
    u32 pulse;
};
void PaletteTransition_BlendBankColors(struct BestiaryPulseDisplay *, u32, u32);
void BestiaryDisplay_UpdatePalettePulse(struct BestiaryPulseDisplay *p)
{
    u32 weight = p->pulse;

    if (weight > 0x1f)
        weight = 0x40 - weight;
    PaletteTransition_BlendBankColors(p, weight, 5);
    p->pulse = (p->pulse + 1) & 0x3f;
}
