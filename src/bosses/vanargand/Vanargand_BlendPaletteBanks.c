#include "global.h"
void Palette_InterpolateBanks(void *, void *, void *, int, int);

void Vanargand_BlendPaletteBanks(u16 (*output)[16], u16 (*from)[16], u16 (*to)[16], int amount,
                                 int shift, int bankCount)
{
    int bank;

    for (bank = 0; bank < bankCount; bank++)
        Palette_InterpolateBanks(output[bank], from[bank], to[bank], amount, shift);
}
