#include "global.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
#define gOrbitEffectRandomIndex gRandomIndex
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
extern const s16 gUnk_086149C4[];
void OrbitEffect_SpawnOrbitTrail(struct OrbitEffect *p)
{
    register u32 timer asm("r0") = p->trailTimer + 1;
    p->trailTimer = timer;
    {
        register u32 mask asm("r2") = 0xffff;
        register u32 work asm("r1");
        asm("" : "+r"(mask));
        work = mask;
        asm("" : "+r"(work));
        timer &= work;
    }
    timer &= 3;
    if (!timer) {
        register u32 fieldOffset asm("r3") = 0x3a0;
        struct OrbitTrailSprite *trail;

        const u16 *table;
        s32 v, delta;
        const s16 *sine;
        struct OrbitEffectPosition *position;
        u32 zero;
        asm("" : "+r"(fieldOffset));
        {
            register u16 *index asm("r0") = (u16 *)((u8 *)p + fieldOffset);
            {
                register u32 entry asm("r1") = *index;
                trail = &p->trails[entry];
            }
        }
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
            const s16 *first = gUnk_086149C4;
            asm("" : "+r"(first));
            v = first[(trail->angle + 64) & 255] * trail->radius;
            sine = first;
        }
        asm("" : "+r"(sine));
        if (v >= 0)
            delta = v >> 12;
        else
            delta = -((-v) >> 12);
        position = trail->target;
        trail->position.x = position->x + delta;
        trail->position.y = position->y;
        v = sine[trail->angle & 255] * trail->radius;
        if (v >= 0)
            delta = v >> 12;
        else
            delta = -((-v) >> 12);
        {
            u16 z = position->z + delta;
            zero = 0;
            trail->position.z = z;
        }
        trail->active = 1;
        trail->age = zero;
        ++p->nextTrail;
        if (p->nextTrail > 11)
            p->nextTrail = zero;
    }
}
