#include "effect_group.h"
#include "engine.h"
#include "ram.h"
// confidence=low: fills the first inactive group with randomized bouncing particles.
#define ParticleGroup_SpawnBouncing sub_08014634
struct Particle {
    u8 active, pad, timer, lifetime;
    u16 cell;
    s16 height;
    struct ContactPosition velocity;
    volatile u32 flags;
    u8 pad14[4];
    u16 tile;
    u8 pad1a[5], priority, pad20[8];
    struct ContactPosition position;
    u8 tail[8];
};
struct Group {
    u8 active, count, type, priority;
    struct Particle particles[8];
};
struct Manager {
    u8 pad[24];
    u32 timer;
    u8 *small, *large;
    struct Group groups[6];
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
#define gRandomTable ((u16 *)0x0203B400)
extern u32 gUnk_08604B90[];
static inline int CellIndex(struct ContactPosition *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0xffff;
    return gTerrainMap->rows[z] + x;
}
struct Manager *ParticleManager_Create(void *, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, s32);
static inline s32 Random(u16 *table, u32 mask, u32 *rng)
{
    u32 r = (*rng + 1) & mask;
    *rng = r;
    r *= 2;
    return *(u16 *)(r + (u32)table);
}
static inline void ClearFlag(volatile u32 *p, u32 mask)
{
    u32 v = *p;
    v &= ~mask;
    *p = v;
}
static inline s32 ReadScalar(const s32 *value) { return *value; }
static inline const struct ContactPosition *ReadOrigin(const struct ContactPosition *const *value) { return *value; }
static inline s32 SpawnInline(s32 count, s32 options, const struct ContactPosition *position,
                                const struct ContactPosition *velocityArg, const struct ContactPosition *spread, s32 lifetime,
                                s32 lifetimeMask)
{
    const struct ContactPosition *velocityInput = velocityArg;
    const struct ContactPosition *velocity;
    struct Manager *m;
    u8 *resource;
    struct Group *g;
    struct Group *pool;
    u32 *rng;
    s32 priority;
    s32 optionMask;
    s32 j;
    s32 i;

    velocity = velocityInput;
    m = gParticleGroupManager;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = ReadScalar(&options) >> 8;
    optionMask = 255;
    options &= optionMask;
    if (ReadScalar(&count) > 8) {
        s32 limit = 8;
        count = limit;
    }

    if (options <= 20) {
        u8 *small = m->small;
        resource = small;
    } else {
        u8 *large = m->large;
        resource = large;
    }
    j = 0;

    rng = &gRandomIndex;
    pool = m->groups;
    g = pool;

    for (; j < 6; g++, pool++, j++) {
        if (!g->active) {
            g->active = 1;
            {
                u32 byte = *((u8 *)&options);
                g->type = byte;
            }
            {
                u32 byte = *((u8 *)&count);
                g->count = byte;
            }
            if (priority > 0)
                g->priority = priority - 1;
            else
                g->priority = 2;
            i = 0;
            {
                s32 bound = count;
                if (i < bound) {
                    u16 *table = (u16 *)0x0203B400;
                    u16 *tableReg = table;
                    struct Particle *p = pool->particles;
                    u8 *sprite = ((u8 *)pool) + 20;
                    u32 mask = 0x3ff;

                    table = tableReg;
                    i = count;

                    do {
                        p->active = 1;
                        p->pad = 1;
                        p->timer = 0;
                        p->lifetime = lifetime + (Random(table, mask, rng) & lifetimeMask);
                        p->cell = CellIndex(ReadOrigin(&position));
                        {
                            const struct ContactPosition *source = position;
                            p->height = source->y;
                        }
                        if (spread->x) {
                            s32 r = Random(table, mask, rng);
                            p->velocity.x = velocity->x + Mod(r, spread->x);
                        } else
                            p->velocity.x = velocity->x;
                        if (spread->y) {
                            s32 r = Random(table, mask, rng);
                            p->velocity.y = velocity->y + Mod(r, spread->y);
                        } else
                            p->velocity.y = velocity->y;
                        if (spread->z) {
                            s32 r = Random(table, mask, rng);
                            p->velocity.z = velocity->z + Mod(r, spread->z);
                        } else
                            p->velocity.z = velocity->z;
                        if ((Random(table, mask, rng) & 15) <= 7)
                            p->flags |= 4;
                        else
                            p->flags &= ~4;
                        ClearFlag(&p->flags, 1);
                        {
                            const struct ContactPosition *source = position;
                            *(u64 *)&p->position = *(const u64 *)source;
                        }
                        {
                            u32 *entries = gUnk_08604B90;
                            s32 type;
                            u32 offset;
                            type = options;
                            offset = type * sizeof(*entries);
                            p->tile = (*((u16 *)(resource + 4))) + *(u32 *)(offset + (u32)entries);
                        }
                        p->priority = g->priority;
                        sub_08217E40(sprite, 0);
                        p++;
                        sprite += 56;

                    } while (--i);
                    i = count;
                }
            }
            if (i < 8) {
                u32 inactive = 0, hidden = 1;
                u32 offset = i * sizeof(struct Particle);
                struct Particle *p = (struct Particle *)(offset + (u32)pool->particles);
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
                                const struct ContactPosition *velocity, const struct ContactPosition *spread,
                                s32 lifetime, s32 lifetimeMask)
{
    return SpawnInline(count, options, position, velocity, spread, lifetime, lifetimeMask);
}
