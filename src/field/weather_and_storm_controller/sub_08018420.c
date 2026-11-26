#include "engine/shared/global.h"
#include "engine/shared/ram.h"

u32 Weather_GetType(void)
{
    return gStormActive;
}
