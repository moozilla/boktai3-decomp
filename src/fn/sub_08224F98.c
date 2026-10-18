// COMPILER: old_agbcc
#include "global.h"
/* SceneLifecycle_DisablePlayTimeCounter: Disables incrementing the persistent counter at game-state offset 0x614 */
#define SceneLifecycle_DisablePlayTimeCounter sub_08224F98

extern u32 gUnk_030053D8;

void sub_08224F98(void)
{
    gUnk_030053D8 = 0;
}
