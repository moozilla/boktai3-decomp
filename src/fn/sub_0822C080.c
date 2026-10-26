#include "global.h"
#include "ram.h"
extern u32 gUnk_030054B0;
u32 Region_OnDestroy(void)
{
    gRegionList = 0;
    gUnk_030054B0 = 0;
    return 0;
}
