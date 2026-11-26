#include "engine/shared/effect_group.h"
#include "effects/particle_groups/particle_group.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
// confidence=low: fills the first inactive group with randomized bouncing particles.
#define ParticleGroup_SpawnBouncing sub_08014634
extern u32 gUnk_08604B90[];
static inline int CellIndex(struct ContactPosition *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0xffff;
    return gTerrainMap->rows[z] + x;
}
struct ParticleGroupManager *ParticleManager_Create(void *, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, s32);
static inline s32 Random(u16 *table, u32 mask, u32 *rng)
{
    u32 r = (*rng + 1) & mask;
    *rng = r;
    r *= 2;
    return *(u16 *)(r + (u32)table);
}
static inline void ClearFlag(u32 *p, u32 mask)
{
    u32 v = *p;
    v &= ~mask;
    *p = v;
}
static inline s32 ReadScalar(const s32 *value)
{
    return *value;
}
static inline const struct ContactPosition *ReadOrigin(const struct ContactPosition *const *value)
{
    return *value;
}
static inline s32 SpawnInline(s32 count, s32 options, const struct ContactPosition *position,
                              const struct ContactPosition *velocityArg,
                              const struct ContactPosition *spread, s32 lifetime, s32 lifetimeMask)
{
    const struct ContactPosition *velocity = velocityArg;
    struct ParticleGroupManager *m = gParticleGroupManager;
    struct ParticleGroup *g;
    struct ParticleGroup *pool;
    u32 *rng;
    s32 priority;
    s32 j;
    s32 i;
    struct ParticleGroupGraphics *resource;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = ReadScalar(&options) >> 8;
    options &= 255;
    if (ReadScalar(&count) > 8)
        count = 8;
    if (options <= 20)
        resource = m->small;
    else
        resource = m->large;
    j = 0;
    rng = &gRandomIndex;
    pool = m->groups;
    g = m->groups;
    for (; j < 6; g++, pool++, j++) {
        if (!g->active) {
            g->active = 1;
            g->type = *((u8 *)&options);
            g->count = *((u8 *)&count);
            if (priority > 0)
                g->priority = priority - 1;
            else
                g->priority = 2;
            i = 0;
            if (i < count) {
                u16 *table = (u16 *)(0x0203B400 + (g->type >> 8));
                struct ParticleGroupParticle *p;
                u8 *sprite;
                u32 randomMask;
                p = pool->particles;
                sprite = (u8 *)&pool->particles[0].flags;
                randomMask = 0x3ff;
                i = count;
                do {
                    p->active = 1;
                    p->unknown1 = 1;
                    p->timer = 0;
                    p->lifetime = lifetime + (Random(table, randomMask, rng) & lifetimeMask);
                    p->cell = CellIndex((struct ContactPosition *)ReadOrigin(&position));
                    p->height = position->y;
                    if (spread->x) {
                        s32 r = Random(table, randomMask, rng);
                        p->velocity.x = velocity->x + Mod(r, spread->x);
                    } else
                        p->velocity.x = velocity->x;
                    if (spread->y) {
                        s32 r = Random(table, randomMask, rng);
                        p->velocity.y = velocity->y + Mod(r, spread->y);
                    } else
                        p->velocity.y = velocity->y;
                    if (spread->z) {
                        s32 r = Random(table, randomMask, rng);
                        p->velocity.z = velocity->z + Mod(r, spread->z);
                    } else
                        p->velocity.z = velocity->z;
                    if ((Random(table, randomMask, rng) & 15) <= 7)
                        p->flags |= 4;
                    else
                        p->flags &= ~4;
                    ClearFlag(&p->flags, 1);
                    p->position = *position;
                    p->tile = resource->tileBase + gUnk_08604B90[options];
                    p->priority = g->priority;
                    sub_08217E40(sprite, 0);
                    p++;
                    sprite += 56;
                } while (--i);
                i = count;
            }
            if (i < 8) {
                u32 inactive = 0, hidden = 1;
                u32 offset = i * sizeof(struct ParticleGroupParticle);
                struct ParticleGroupParticle *p =
                    (struct ParticleGroupParticle *)(offset + (u32)pool->particles);
                do {
                    p->active = inactive;
                    p->flags |= hidden;
                    p++;
                    i++;
                } while (i < 8);
            }
            break;
        }
    }
    return 0;
}
s32 ParticleGroup_SpawnBouncing(s32 count, s32 options, const struct ContactPosition *position,
                                const struct ContactPosition *velocity,
                                const struct ContactPosition *spread, s32 lifetime,
                                s32 lifetimeMask)
{
    return SpawnInline(count, options, position, velocity, spread, lifetime, lifetimeMask);
}
