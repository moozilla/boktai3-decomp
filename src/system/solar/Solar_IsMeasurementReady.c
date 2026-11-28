#include "global.h"
#include "engine/ram.h"
#include "engine/runtime.h"
#include "system/solar/solar_measurement_task.h"
/* Solar_IsMeasurementReady: Checks that the solar task is in measurement state and is not paused */
u32 Solar_IsMeasurementReady(void)
{
    struct SolarMeasurementTask *task = (void *)gSolarController;
    if (task && !gSolarPaused && task->state == 2)
        return 1;
    return 0;
}
