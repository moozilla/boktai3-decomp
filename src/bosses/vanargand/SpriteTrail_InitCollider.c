#include "bosses/vanargand/sprite_trail.h"
#include "scene/collision/spatial_index.h"

void VanargandHead_OnEffectMotionComplete(void *, struct SpatialEntry *);
void SpriteTrail_InitCollider(struct SpriteTrail *p)
{
    struct ContactPosition position, size, offset;
    struct SpatialEntry *collider = (void *)p->collider;
    u32 mask = 5;
    position.x = p->position.x >> 12;
    position.y = p->position.y >> 12;
    position.z = p->position.z >> 12;
    size.x = 4;
    size.y = 4;
    size.z = 4;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 12, mask, 1, p->collisionArg0,
                           p->collisionArg1, VanargandHead_OnEffectMotionComplete, p);
}
