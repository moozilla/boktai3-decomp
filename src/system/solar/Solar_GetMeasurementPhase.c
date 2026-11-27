// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* Solar_GetMeasurementPhase: Returns the interrupt state machine phase used during sensor conversion */

u32 Solar_GetMeasurementPhase(void)
{
    return gSolarMeasurementPhase;
}
