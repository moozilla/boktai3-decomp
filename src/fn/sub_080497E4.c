// COMPILER: old_agbcc
#include "global.h"
/* TetherActor_ClearInstance: Clears the global tether actor pointer without destroying the actor */
#define TetherActor_ClearInstance sub_080497E4

extern u32 gUnk_02000488;

void sub_080497E4(void)
{
    gUnk_02000488 = 0;
}
