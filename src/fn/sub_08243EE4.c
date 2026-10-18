// COMPILER: old_agbcc
#include "global.h"
/* Solar_GetMeasurementPhase: Returns the interrupt state machine phase used during sensor conversion */
#define Solar_GetMeasurementPhase sub_08243EE4

extern u32 gUnk_030035B0;

u32 sub_08243EE4(void)
{
    return gUnk_030035B0;
}
