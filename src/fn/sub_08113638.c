// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* SceneDebrisPool_ResetSingleton: Clears the debris pool singleton pointer */
#define SceneDebrisPool_ResetSingleton sub_08113638

void sub_08113638(void)
{
    gDebrisPool = 0;
}
