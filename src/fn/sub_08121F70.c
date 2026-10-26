// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* SceneSpriteBurst_ResetSingleton: Clears the shared burst manager pointer */
#define SceneSpriteBurst_ResetSingleton sub_08121F70

void sub_08121F70(void)
{
    gSpriteGroup = 0;
}
