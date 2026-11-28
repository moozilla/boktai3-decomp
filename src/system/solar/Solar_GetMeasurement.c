#include "global.h"
#include "engine/runtime.h"
s32 Solar_GetMeasurement(void)
{
    return gSolarMeasuring != 0 ? gSolarMeasurement : -1;
}
