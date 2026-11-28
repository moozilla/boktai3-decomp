#include "global.h"
#include "engine/ram.h"
#include "link/session/session_save.h"
#include "system/solar/solar_measurement_task.h"
s32 Solar_GetMeasurement(void);
s32 Solar_CaptureBaseline(void)
{
    if (((struct SolarMeasurementTask *)gSolarController)->state == 2) {
        s32 measurement = Solar_GetMeasurement();
        if (measurement >= 0) {
            s32 baseline = measurement - 2;

            ((struct SessionSave *)gSessionState)->solarBaseline = baseline;
            return 1;
        }
    }
    return 0;
}
