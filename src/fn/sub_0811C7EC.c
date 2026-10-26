#include "global.h"
#include "render.h"
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void CharacterModel_LoadPrimaryPalette(u8 *s)
{
    s32 i = CharacterModel_GetPrimaryPaletteIndex(s);
    CpuSet(gPaletteBankData + (i << 5), s + 0x210, 0x10);
}
