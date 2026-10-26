#include "global.h"
#include "render.h"
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void CharacterModel_InitPalettes(u8 *s, s32 n)
{
    u8 *a, *b;
    u8 *d;
    s32 i;
    u32 c;
    a = gPaletteBankData + (CharacterModel_GetPrimaryPaletteIndex(s) << 5);
    b = gPaletteBankData + ((n + 0x379) << 5);
    d = s + 0x210;
    CpuSet(a, d, 0x10);
    CpuSet(b, s + 0x230, 0x10);
    c = 0xa000;
    i = 4;
    do {
        *(u32 *)(s + 0x48) = (u32)d;
        *(u16 *)(s + 0x3a) = c;
        s += 0x60;
        i--;
    } while (i >= 0);
}
