// COMPILER: old_agbcc
#include "global.h"
#include "engine/runtime.h"
/* SceneLifecycle_DisablePlayTimeCounter: Disables incrementing the persistent counter at game-state offset 0x614 */

void SceneLifecycle_DisablePlayTimeCounter(void)
{
    gPlayTimeCounterEnabled = 0;
}
