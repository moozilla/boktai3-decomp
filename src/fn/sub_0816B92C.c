#include "global.h"
extern u8 *gUnk_02000214;
void MapScreen_SuppressSpecialMarkers(void)
{
    if (gUnk_02000214)
        *(u32 *)(gUnk_02000214 + 0x4928) = 1;
}
