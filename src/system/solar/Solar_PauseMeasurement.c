#include "global.h"
#include "engine/runtime.h"
#include "engine/ram.h"
#include "system/solar/solar_measurement_task.h"
void Solar_StopMeasurement(void);
void Solar_PauseMeasurement(void)
{
    if (gSolarController) {
        if (((struct SolarMeasurementTask *)gSolarController)->state)
            Solar_StopMeasurement();
        gSolarPaused = 1;
    }
}
