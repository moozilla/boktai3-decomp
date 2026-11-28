#include "global.h"
#include "engine/runtime.h"
#include "system/solar/solar_measurement_task.h"
void Callback_InvokeActor(struct SolarController *, void (*)(struct SolarMeasurementTask *));
u32 Solar_UpdateTask(struct SolarController *controller)
{
    if (gSolarPaused == 0)
        Callback_InvokeActor(controller, controller->mode);
    return 0;
}
