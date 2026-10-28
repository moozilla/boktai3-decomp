// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* SceneLifecycle_EnablePlayTimeCounter: Enables incrementing the persistent counter at game-state offset 0x614 */
#define SceneLifecycle_EnablePlayTimeCounter sub_08224F8C

void sub_08224F8C(void)
{
    gPlayTimeCounterEnabled = 1;
}
