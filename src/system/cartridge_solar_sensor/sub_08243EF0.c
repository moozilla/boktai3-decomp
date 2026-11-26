// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
/* Solar_GetInterruptCounter: Returns the counter incremented on each active sensor interrupt */
#define Solar_GetInterruptCounter sub_08243EF0

u32 sub_08243EF0(void)
{
    return gSolarInterruptCounter;
}
