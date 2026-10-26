#include "global.h"
#include "ram.h"

void SceneEffect863A_LoadResources(void);

s32 SceneSpriteBurst_InitManager(s32 value)
{
    SceneEffect863A_LoadResources();
    gSpriteGroup = value;
    return 0;
}
