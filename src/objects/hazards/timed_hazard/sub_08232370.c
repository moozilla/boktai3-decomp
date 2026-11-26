// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* TimedHazard_ResetManager: Clears the singleton for the following timed collider object family; identity tentative */

void TimedHazard_ResetManager(void)
{
    gColliderSpritePool = 0;
}
