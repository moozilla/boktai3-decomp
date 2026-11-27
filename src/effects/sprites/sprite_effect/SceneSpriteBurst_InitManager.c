#include "global.h"
#include "engine/ram.h"

void SpriteEffectSet_LoadResources(s32);

s32 SceneSpriteBurst_InitManager(s32 value)
{
    SpriteEffectSet_LoadResources(value);
    gSpriteGroup = value;
    return 0;
}
