#include "global.h"
#include "ram.h"

u32 AnimData_FindEntry(u32 a);

u32 ShadowTask_Init(u8 *p)
{
    gShadowManager = p;
    *(u32 *)(p + 0x1C) = 0;
    *(u32 *)(p + 0x18) = AnimData_FindEntry(0x1C1A);
    return 0;
}
