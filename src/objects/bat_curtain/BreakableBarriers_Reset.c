// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* BreakableBarriers_Reset: Zeroes the module-level gShadowManager global, resetting barrier subsystem state */

void BreakableBarriers_Reset(void)
{
    gShadowManager = 0;
}
