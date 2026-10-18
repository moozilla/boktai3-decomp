// COMPILER: old_agbcc
#include "global.h"
/* SceneLifecycle_EnablePlayTimeCounter: Enables incrementing the persistent counter at game-state offset 0x614 */
#define SceneLifecycle_EnablePlayTimeCounter sub_08224F8C

extern u32 gUnk_030053D8;

void sub_08224F8C(void)
{
    gUnk_030053D8 = 1;
}
