// COMPILER: old_agbcc
#include "global.h"
/* TimedHazard_ResetManager: Clears the singleton for the following timed collider object family; identity tentative */
#define TimedHazard_ResetManager sub_08232370

extern u32 gUnk_02000474;

void sub_08232370(void)
{
    gUnk_02000474 = 0;
}
