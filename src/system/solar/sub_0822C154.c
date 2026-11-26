#include "global.h"
#include "engine/ram.h"
#include "engine/runtime.h"
/* Solar_IsMeasurementReady: Checks that the solar task is in measurement state and is not paused */
u32 Solar_IsMeasurementReady(void)
{
    u8 *p = gSolarController;
    if (p && !gSolarPaused && p[0x19] == 2)
        return 1;
    return 0;
}
