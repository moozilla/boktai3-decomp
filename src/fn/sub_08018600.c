#include "global.h"
#include "ram.h"

void PaletteCycle_RefreshBasePalette(u8 *p);

u32 PaletteCycle_Initialize(u8 *p)
{
    gPaletteCycle = p;
    *(u32 *)(p + 0x18) = 0;
    PaletteCycle_RefreshBasePalette(p);
    return 0;
}
