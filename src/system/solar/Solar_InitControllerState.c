// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
#include "system/game_state/game_state.h"
#include "system/solar/solar_measurement_task.h"
void Solar_UpdateMeasurement(struct SolarMeasurementTask *);
void Solar_InitControllerState(struct SolarController *controller)
{
    ((u8 *)controller)[0x18] = 1;
    controller->mode = Solar_UpdateMeasurement;
    controller->measurement.state = 0;
    controller->measurement.count = 0;
    controller->measurement.debugRepeat = 0;
    *(u16 *)&gGameState->unk874 = gSolarSavedLevel;
    *(u16 *)&gGameState->unk876 = gSolarSavedCachedLevel;
}
