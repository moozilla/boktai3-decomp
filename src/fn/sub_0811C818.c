#include "global.h"
#include "render.h"
void CharacterModel_LoadSecondaryPalette(u8 *a, s32 b)
{
    u8 **gp;
    u32 k = b + 0x379;
    gp = &gPaletteBankData;
    CpuSet(*gp + (k << 5), a + 0x230, 0x10);
}
