// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* TimedHazard_ResetManager: Clears the singleton for the following timed collider object family; identity tentative */
#define TimedHazard_ResetManager sub_08232370

void sub_08232370(void)
{
    gColliderSpritePool = 0;
}
