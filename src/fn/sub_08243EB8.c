#include "global.h"
#include "runtime.h"
s32 Solar_GetMeasurement(void)
{
    s32 r;
    if (gSolarMeasuring != 0)
        r = gSolarMeasurement;
    else
        r = -1;
    return r;
}
