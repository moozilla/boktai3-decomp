#include "global.h"

void Mosaic_ResetParameters(void);
extern u32 gUnk_02000070;

u32 PaletteLevels_Destroy(void)
{
    Mosaic_ResetParameters();
    gUnk_02000070 = 0;
    return 0;
}
