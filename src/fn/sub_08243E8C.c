#include "global.h"
#include "runtime.h"
extern u32 gUnk_03006A38;
extern s32 gUnk_03006A48;
void Solar_Disable(void);
void Solar_StopMeasurement(void)
{
    if (gSolarEnabled == 1)
        Solar_Disable();
    gUnk_03006A38 = 0;
    gUnk_03006A48 = -1;
}
