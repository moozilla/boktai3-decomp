#define SpriteComponent_GetCollisionStatus sub_081806BC
#include "global.h"
struct SpriteComponent {
    u8 unknown00[0x5A];
    s16 collisionStatus;
};
s32 sub_081806BC(struct SpriteComponent *component)
{
    return component->collisionStatus;
}
