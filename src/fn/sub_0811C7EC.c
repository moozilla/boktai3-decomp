#include "global.h"
extern u8 *gUnk_030042E4;
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void CharacterModel_LoadPrimaryPalette(u8 *s)
{
    s32 i = CharacterModel_GetPrimaryPaletteIndex(s);
    CpuSet(gUnk_030042E4 + (i << 5), s + 0x210, 0x10);
}
