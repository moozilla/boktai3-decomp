// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* Solar_GetInterruptCounter: Returns the counter incremented on each active sensor interrupt */

u32 Solar_GetInterruptCounter(void)
{
    return gSolarInterruptCounter;
}
