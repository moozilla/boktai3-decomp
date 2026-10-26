#include "global.h"
#include "ram.h"

u32 ShadowRegistry_InitTask(u8 *p)
{
    gShadowRegistry = p;
    *(u32 *)(p + 0x18) = 0;
    return 0;
}
