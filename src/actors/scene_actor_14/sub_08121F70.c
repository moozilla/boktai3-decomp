// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* SceneSpriteBurst_ResetSingleton: Clears the shared burst manager pointer */
#define SceneSpriteBurst_ResetSingleton sub_08121F70

void sub_08121F70(void)
{
    gSpriteGroup = 0;
}
