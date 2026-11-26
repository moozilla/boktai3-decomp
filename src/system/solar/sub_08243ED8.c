// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* Solar_IsEnabled: Returns the software flag set by Enable and cleared by Disable */

u32 Solar_IsEnabled(void)
{
    return gSolarEnabled;
}
