#include "bosses/vanargand/sprite_trail.h"
#include "scene/collision/spatial_index.h"

void SingletonSlot7_OnEffectMotionComplete(void *owner, struct SpatialEntry *other)
{
    struct SpriteTrail *trail = owner;
    u8 one = 1;
    trail->state = one;
    trail->entered = one;
    trail->frame = 0;
}
