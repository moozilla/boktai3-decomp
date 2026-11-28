#include "bosses/vanargand/sprite_trail.h"
#include "scene/collision/spatial_index.h"
#include "bosses/vanargand/flag_accessors.h"

void SpriteTrail_UpdatePositions(struct SpriteTrail *);
s32 SceneTilemap_GetScrollDelta(void);
void *VanargandHead_CreateImpactSprite(void *, s32, s32);
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
            SpatialEntry_SetFlags(collider, 1);
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
    VanargandHead_CreateImpactSprite(&screen, 6, delta);
    p->state = 2;
    p->entered = 1;
    p->frame = 1;
}
