// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* BreakableBarriers_Reset: Zeroes the module-level gShadowManager global, resetting barrier subsystem state */
#define BreakableBarriers_Reset sub_080133C0

void sub_080133C0(void)
{
    gShadowManager = 0;
}
