#include "global.h"
#include "ram.h"

u32 ScaledShadowManager_Init(u8 *p)
{
    gScaledShadowManager = p;
    *(u32 *)(p + 0x1C) = 0;
    return 0;
}
