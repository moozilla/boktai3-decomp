#include "effects/sprite_trail/sprite_trail.h"
#include "engine/spatial_index/spatial_index.h"

void MotionEffectPool_OnRenderComplete(void *, struct SpatialEntry *);
void SpriteTrail_InitRenderCollider(struct SpriteTrail *p)
{
    struct ContactPosition position, size, offset;
    struct SpatialEntry *collider = (void *)p->renderCollider;
    u32 mask = 5;
    position.x = p->position.x >> 12;
    position.y = p->position.x >> 12;
    position.z = p->position.x >> 12;
    size.x = 4;
    size.y = 4;
    size.z = 4;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    SpatialIndex_InitEntry(collider, &position, &size, &offset, 20, mask, 1, 0, 0,
                           MotionEffectPool_OnRenderComplete, p);
    SpatialIndex_InsertEntry(collider);
}
