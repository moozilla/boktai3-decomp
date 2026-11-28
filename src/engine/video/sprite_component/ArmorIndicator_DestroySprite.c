#include "global.h"
#include "engine/video/sprite_component.h"

void Sprite_Remove(struct DisplaySequenceSprite *);

void ArmorIndicator_DestroySprite(struct SpriteComponent *component)
{
    Sprite_Remove(&component->sprite);
}
