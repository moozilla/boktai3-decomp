// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
/* Solar_GetMeasurementPhase: Returns the interrupt state machine phase used during sensor conversion */
#define Solar_GetMeasurementPhase sub_08243EE4

u32 sub_08243EE4(void)
{
    return gSolarMeasurementPhase;
}
