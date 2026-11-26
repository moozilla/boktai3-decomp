#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
void Solar_Disable(void);
void Solar_StopMeasurement(void)
{
    if (gSolarEnabled == 1)
        Solar_Disable();
    gSolarMeasuring = 0;
    gSolarMeasurement = -1;
}
