// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* Solar_GetMeasurementPhase: Returns the interrupt state machine phase used during sensor conversion */
#define Solar_GetMeasurementPhase sub_08243EE4

u32 sub_08243EE4(void)
{
    return gSolarMeasurementPhase;
}
