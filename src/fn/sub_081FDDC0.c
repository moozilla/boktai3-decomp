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
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
extern const s16 gUnk_086149C4[];
static inline s32 TrailComponent(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void OrbitEffect_SpawnOrbitTrail(struct OrbitEffect *p)
{
    if (!(++p->trailTimer & 3)) {
        struct OrbitTrailSprite *trail = &p->trails[p->nextTrail];
        const u16 *table;
        if (!p->hidden)
            trail->flags &= ~1;
        trail->flags &= ~0x200;
        trail->target = &p->position;
        table = gRandomTable;
        trail->radius = (RandomValue(table) & 511) + 768;
        trail->angle = (RandomValue(table) >> 4) & 255;
        if (p->kind == 2)
            SpriteDrawable_SelectTileFrame(trail, p->resource, 1);
        else {
            SpriteDrawable_SelectTileFrame(trail, p->resource, 5);
            Effect_SetPalette(trail, 1);
        }
        {
            s32 delta;
            delta = TrailComponent(gUnk_086149C4[(trail->angle + 64) & 255] * trail->radius);
            trail->position.x = trail->target->x + delta;
            trail->position.y = trail->target->y;
            delta = TrailComponent(gUnk_086149C4[trail->angle & 255] * trail->radius);
            trail->position.z = trail->target->z + delta;
        }
        trail->active = 1;
        trail->age = 0;
        if (++p->nextTrail > 11)
            p->nextTrail = 0;
    }
}
