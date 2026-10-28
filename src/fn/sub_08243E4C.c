#include "global.h"
#include "runtime.h"
void Solar_Enable(void);
void Solar_StartMeasurement(void)
{
    if (gSolarEnabled == 0)
        Solar_Enable();
    gSolarMeasuring = 1;
    gSolarMeasurementPhase = 0;
    gSolarInterruptCounter = 0;
    gSolarMeasurement = -1;
}
