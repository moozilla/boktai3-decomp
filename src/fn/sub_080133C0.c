// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* BreakableBarriers_Reset: Zeroes the module-level gShadowManager global, resetting barrier subsystem state */
#define BreakableBarriers_Reset sub_080133C0

void sub_080133C0(void)
{
    gShadowManager = 0;
}
