#include "global.h"
u8 *ResourceRegistry_GetResource(u32);
u8 *Task4E01_GetEntryPosition(s32 i)
{
    u8 *p = ResourceRegistry_GetResource(0x4E01);
    if (p == 0 || i < 0 || i >= *(s32 *)(p + 0x3c))
        return 0;
    return *(u8 **)(p + 0x108) + i * 0x1ac + 0xa8;
}
