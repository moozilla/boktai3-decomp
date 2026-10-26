#include "global.h"
#include "render.h"

void Palette_CopyBank(u32, u8 *);

void Armor_CopyScenePalette(u32 a)
{
    Palette_CopyBank(a, gPaletteBankData + 0x3240);
}
