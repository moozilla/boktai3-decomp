#include "global.h"
struct SpriteComponent {
    u8 unknown00[0x5A];
    s16 collisionStatus;
};
s32 SpriteComponent_GetCollisionStatus(struct SpriteComponent *component)
{
    return component->collisionStatus;
}
