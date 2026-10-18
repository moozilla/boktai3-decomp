#include "global.h"

extern u8 *gUnk_02000094;
void PaletteCycle_RefreshBasePalette(u8 *p);

u32 PaletteCycle_Initialize(u8 *p)
{
    gUnk_02000094 = p;
    *(u32 *)(p + 0x18) = 0;
    PaletteCycle_RefreshBasePalette(p);
    return 0;
}
