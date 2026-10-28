// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* Solar_IsEnabled: Returns the software flag set by Enable and cleared by Disable */
#define Solar_IsEnabled sub_08243ED8

u32 sub_08243ED8(void)
{
    return gSolarEnabled;
}
