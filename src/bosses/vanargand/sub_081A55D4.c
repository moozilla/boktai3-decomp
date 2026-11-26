#include "engine/shared/global.h"
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void Vanargand_BlendPaletteBanks(u8 *output, u8 *from, u8 *to, int amount, int shift,
                                  int bankCount)
{
    u16 (*outputBank)[16] = (u16 (*)[16])output;
    u16 (*firstBank)[16] = (u16 (*)[16])from;
    u16 (*secondBank)[16] = (u16 (*)[16])to;
    int bank;
    for (bank = 0; bank < bankCount; bank++) {
        Palette_InterpolateBanks(outputBank[bank], firstBank[bank], secondBank[bank],
                                 amount, shift);
    }
}
