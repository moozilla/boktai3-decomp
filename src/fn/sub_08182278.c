#include "global.h"

extern u8 *gUnk_030042E4;
void Palette_CopyBank(u32, u8 *);

void Armor_CopyScenePalette(u32 a)
{
    Palette_CopyBank(a, gUnk_030042E4 + 0x3240);
}
