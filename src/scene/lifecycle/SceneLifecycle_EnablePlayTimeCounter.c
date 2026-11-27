// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* SceneLifecycle_EnablePlayTimeCounter: Enables incrementing the persistent counter at game-state offset 0x614 */

void SceneLifecycle_EnablePlayTimeCounter(void)
{
    gPlayTimeCounterEnabled = 1;
}
