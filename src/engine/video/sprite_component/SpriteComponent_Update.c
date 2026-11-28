#include "global.h"
#include "engine/engine.h"
#include "engine/video/sprite_component.h"

void SpriteComponent_Update(struct SpriteComponent *component)
{
    const s8 baseOrder[1] = {-96};

    if (component->sprite.flags & 1)
        Sound_PlaySong(0x202);
    component->sprite.flags &= ~1;
    component->sprite.position = component->position;
    component->sprite.attribute = baseOrder[0] - (u8)component->frame;
    component->collider.position = component->position;
    Collider_Refresh(&component->collider);
}
