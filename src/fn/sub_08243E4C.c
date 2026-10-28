#include "global.h"
#include "runtime.h"
extern u32 gUnk_03006A38;
extern s32 gUnk_03006A48;
void Solar_Enable(void);
void Solar_StartMeasurement(void)
{
    if (gSolarEnabled == 0)
        Solar_Enable();
    gUnk_03006A38 = 1;
    gSolarMeasurementPhase = 0;
    gSolarInterruptCounter = 0;
    gUnk_03006A48 = -1;
}
