// COMPILER: old_agbcc
#include "global.h"
/* SceneDebrisPool_ResetSingleton: Clears the debris pool singleton pointer */
#define SceneDebrisPool_ResetSingleton sub_08113638

extern u32 gUnk_020004C4;

void sub_08113638(void)
{
    gUnk_020004C4 = 0;
}
