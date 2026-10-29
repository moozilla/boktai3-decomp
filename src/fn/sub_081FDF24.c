#include "global.h"
#include "engine.h"
#include "ram.h"
struct OrbitEffectPosition {
    s16 x, y, z, pad;
};
struct OrbitTrailSprite {
    u32 flags;
    u8 unknown4[0x18 - 4];
    struct OrbitEffectPosition position;
    u8 unknown20[8];
    struct OrbitEffectPosition *target;
    u16 active, age;
    s16 angle, radius, velocityX, velocityZ;
};
struct OrbitEffect {
    u8 unknown0[0x18];
    s16 kind;
    u8 unknown1A[0x50 - 0x1a];
    struct OrbitEffectPosition position;
    u8 unknown58[0xfa - 0x58];
    s16 hidden;
    void *resource;
    struct OrbitTrailSprite trails[12];
    u16 nextTrail, trailTimer;
};
void SpriteDrawable_SelectTileFrame(struct OrbitTrailSprite *, void *, u32);
void Effect_SetPalette(struct OrbitTrailSprite *, u32);
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
void OrbitEffect_SpawnHomingTrail(struct OrbitEffect *p, struct OrbitEffectPosition *origin)
{
    if (!(++p->trailTimer & 3)) {
        struct OrbitTrailSprite *trail = &p->trails[p->nextTrail];
        const u16 *table;
        if (!p->hidden)
            trail->flags &= ~1;
        trail->flags &= ~0x200;
        trail->target = &p->position;
        table = gRandomTable;
        trail->radius = (RandomValue(table) & 15) + 15;
        trail->angle = (RandomValue(table) >> 4) & 255;
        if (p->kind == 2)
            SpriteDrawable_SelectTileFrame(trail, p->resource, 1);
        else {
            SpriteDrawable_SelectTileFrame(trail, p->resource, 5);
            Effect_SetPalette(trail, 1);
        }
        trail->position.x = origin->x;
        trail->position.y = origin->y;
        trail->position.z = origin->z;
        trail->active = 1;
        trail->age = 0;
        if (++p->nextTrail > 11)
            p->nextTrail = 0;
    }
}
