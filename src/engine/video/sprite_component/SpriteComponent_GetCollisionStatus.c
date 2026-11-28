#include "global.h"
#include "engine/video/sprite_component.h"

s32 SpriteComponent_GetCollisionStatus(struct SpriteComponent *component)
{
    return component->collisionStatus;
}
