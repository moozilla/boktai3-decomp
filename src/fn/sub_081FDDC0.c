#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.4%
// Remaining differences: timer mask copy, sine-table pointer lifetime, and registers.
#define gOrbitEffectRandomTable gUnk_0203B400
#define gOrbitEffectRandomIndex gUnk_03005308
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
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void SpriteDrawable_SelectTileFrame(struct OrbitTrailSprite *, void *, u32);
void Effect_SetPalette(struct OrbitTrailSprite *, u32);
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}
extern const s16 gUnk_086149C4[];
void OrbitEffect_SpawnOrbitTrail(struct OrbitEffect *p)
{
    u32 timer = p->trailTimer + 1;
    p->trailTimer = timer;
    {
        u16 mask = 0xffff;
        timer &= mask;
    }
    if (!(timer & 3)) {
        struct OrbitTrailSprite *trail = &p->trails[p->nextTrail];
        const u16 *table;
        u32 zero;
        if (!p->hidden)
            trail->flags &= ~1;
        trail->flags &= ~0x200;
        trail->target = &p->position;
        table = gUnk_0203B400;
        trail->radius = (RandomValue(table) & 511) + 768;
        trail->angle = (RandomValue(table) >> 4) & 255;
        if (p->kind == 2)
            SpriteDrawable_SelectTileFrame(trail, p->resource, 1);
        else {
            SpriteDrawable_SelectTileFrame(trail, p->resource, 5);
            Effect_SetPalette(trail, 1);
        }
        {
            struct OrbitEffectPosition *position;
            const s16 *sine = gUnk_086149C4;
            s32 v, delta;
            v = sine[(trail->angle + 64) & 255] * trail->radius;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            position = trail->target;
            trail->position.x = (u16)position->x + delta;
            trail->position.y = position->y;
            v = sine[trail->angle & 255] * trail->radius;
            if (v >= 0)
                delta = v >> 12;
            else
                delta = -((-v) >> 12);
            {
                u16 z = (u16)position->z + delta;
                zero = 0;
                trail->position.z = z;
            }
            trail->active = 1;
            trail->age = zero;
            p->nextTrail++;
            if (p->nextTrail > 11)
                p->nextTrail = zero;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", OrbitEffect_SpawnOrbitTrail);
#endif
