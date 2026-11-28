#include "global.h"
#include "engine/runtime.h"
#include "engine/ram.h"
#include "system/solar/solar_measurement_task.h"
void Solar_StartMeasurement(void);
void Solar_ResumeMeasurement(void)
{
    struct SolarMeasurementTask *task = (void *)gSolarController;
    if (task) {
        if (task->state != 0) {
            task->state = 1;
            ((struct SolarMeasurementTask *)gSolarController)->count = 0;
            Solar_StartMeasurement();
        }
        gSolarPaused = 0;
    }
}
