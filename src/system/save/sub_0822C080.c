#include "global.h"
#include "engine/ram.h"
u32 Region_OnDestroy(void)
{
    gRegionList = 0;
    gRegionOverride = 0;
    return 0;
}
