#include "global.h"
#include "engine.h"

struct ArmorSpriteColliderPart {
    u8 sprite[0x9C];
    u8 collider[0x1E4 - 0x9C];
};

void Armor6_DestroySpriteColliders(struct ArmorSpriteColliderPart *parts)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        Sprite_Remove(parts[i].sprite);
        Collider_Remove(parts[i].collider);
    }
}
