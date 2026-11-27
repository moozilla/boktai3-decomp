// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* SceneSpriteBurst_ResetSingleton: Clears the shared burst manager pointer */

void SceneSpriteBurst_ResetSingleton(void)
{
    gSpriteGroup = 0;
}
