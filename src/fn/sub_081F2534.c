#include "sprite_trail.h"
#include "spatial_index.h"

void SpriteTrail_UpdatePositions(struct SpriteTrail *);
s32 SceneTilemap_GetScrollDelta(void);
void *SingletonSlot7_CreateImpactSprite(void *, s32, s32);
void SpriteTrail_BeginImpact(void *unused, struct SpriteTrail *p)
{
    struct ContactPosition screen;
    s32 delta, y, z;
    s16 *out;
    struct TrailVector *pos;
    if (p->entered) {
        p->entered = 0;
        {
            struct SpatialEntry *collider = (void *)p->renderCollider;
            u32 one = 1;
            collider->flags |= one;
        }
    }
    SpriteTrail_UpdatePositions(p);
    delta = SceneTilemap_GetScrollDelta() >> 12;
    pos = &p->position;
    out = &screen.x;
    screen.x = (pos->x >> 12) - ((struct TilemapPanel *)gSceneCamera)->scroll.x;
    y = pos->y;
    z = pos->z;
    out[1] = ((z + y) >> 12) + 0x60;
    out[2] = pos->z >> 12;
    SingletonSlot7_CreateImpactSprite(&screen, 6, delta);
    p->state = 2;
    p->entered = 1;
    p->frame = 1;
}
