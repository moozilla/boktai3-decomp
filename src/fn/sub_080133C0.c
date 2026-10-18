// COMPILER: old_agbcc
#include "global.h"
/* BreakableBarriers_Reset: Zeroes the module-level gUnk_02000054 global, resetting barrier subsystem state */
#define BreakableBarriers_Reset sub_080133C0

extern u32 gUnk_02000054;

void sub_080133C0(void)
{
    gUnk_02000054 = 0;
}
