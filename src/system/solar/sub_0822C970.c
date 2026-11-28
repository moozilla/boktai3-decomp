#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/runtime.h"
#include "system/solar/solar_measurement_task.h"
void Solar_StartMeasurement(void), sub_0822C598(struct SolarMeasurementTask *);
u16 sub_0822C8D8(struct SolarMeasurementTask *);
s32 Solar_GetIntensity(void), Solar_GetLevelForIntensity(s32), Solar_ApplyIntensityModifiers(s32);
void sub_0822C970(struct SolarMeasurementTask *obj)
{
    switch (obj->state) {
        case 0:
            if (++obj->count > 29) {
                Solar_StartMeasurement();
                obj->state = 1;
                obj->count = 0;
            }
            break;
        case 1:
            if (++obj->count > 59) {
                obj->state = 2;
                obj->count = 0;
            }
            break;
        case 2:
            obj->a = sub_0822C8D8(obj);
            obj->gain = Solar_GetLevelForIntensity(obj->a);
            *(u16 *)&gGameState->unk874 = Solar_ApplyIntensityModifiers(obj->a);
            *(u16 *)&gGameState->unk876 = Solar_GetLevelForIntensity(gGameState->unk874);
            sub_0822C598(obj);
            gSolarSavedLevel = gGameState->unk874;
            gSolarSavedCachedLevel = gGameState->unk876;
            break;
    }
}
