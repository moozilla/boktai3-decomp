#include "global.h"
extern u32 gUnk_03002600;
extern u32 gUnk_030054B0;
u32 Region_OnDestroy(void)
{
    gUnk_03002600 = 0;
    gUnk_030054B0 = 0;
    return 0;
}
