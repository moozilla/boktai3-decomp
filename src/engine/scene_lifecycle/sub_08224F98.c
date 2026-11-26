// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
/* SceneLifecycle_DisablePlayTimeCounter: Disables incrementing the persistent counter at game-state offset 0x614 */
#define SceneLifecycle_DisablePlayTimeCounter sub_08224F98

void sub_08224F98(void)
{
    gPlayTimeCounterEnabled = 0;
}
