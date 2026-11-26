#include "engine/shared/global.h"
#include "engine/shared/ram.h"

void SceneEffect863A_LoadResources(s32);

s32 SceneSpriteBurst_InitManager(s32 value)
{
    SceneEffect863A_LoadResources(value);
    gSpriteGroup = value;
    return 0;
}
