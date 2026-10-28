// COMPILER: old_agbcc
#include "global.h"
#include "runtime.h"
/* SceneLifecycle_DisablePlayTimeCounter: Disables incrementing the persistent counter at game-state offset 0x614 */
#define SceneLifecycle_DisablePlayTimeCounter sub_08224F98

void sub_08224F98(void)
{
    gPlayTimeCounterEnabled = 0;
}
