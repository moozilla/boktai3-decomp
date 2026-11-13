#include "effect_group.h"
// MATCHING_CONCESSION: register-pinning
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
struct SpawnArgs {
    s32 count, options;
    const struct ContactPosition *position;
};
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
s32 ParticleGroup_SpawnBouncing(s32 count, s32 options, const struct ContactPosition *position,
                                const struct ContactPosition *velocityArg,
                                const struct ContactPosition *spread, s32 lifetime,
                                s32 lifetimeMask)
{
    struct SpawnArgs args = {count, options, position};
    const struct ContactPosition *velocityInput = velocityArg;
    const struct ContactPosition *velocity;
    union ManagerValue {
        struct Manager *manager;
        struct SpawnArgs *header;
        u32 byte;
    } managerValue;
    u8 *resource;
    struct Group *g;
    struct Group *pool;
    u32 *rng;
    union ParticleValue {
        s32 priority;
        struct Particle *particle;
    } particleValue;
    s32 optionMask;
    union ArgumentValue {
        s32 value;
        struct SpawnArgs *header;
        const struct ContactPosition *position;
    } option;
    s32 j;
    register s32 i asm("r5");

    velocity = velocityInput;
    managerValue.manager = gParticleGroupManager;
    if (!managerValue.manager) {
        managerValue.manager = ParticleManager_Create(0, 0);
        if (!managerValue.manager)
            return -1;
    }
    particleValue.priority = args.options >> 8;
    asm("" : "+m"(args.options));
    optionMask = 255;
    option.value = args.options;
    option.value &= optionMask;
    args.options = option.value;
    if (args.count > 8) {
        option.value = 8;
        args.count = option.value;
    }

    if (args.options <= 20) {
        register u8 *small asm("r2") = managerValue.manager->small;
        asm("" : "+r"(small));
        resource = small;
    } else {
        register u8 *large asm("r0") = managerValue.manager->large;
        asm("" : "+r"(large));
        resource = large;
    }
    j = 0;

    rng = &gRandomIndex;
    pool = managerValue.manager->groups;
    g = pool;

    for (; j < 6; g++, pool++, j++) {
        if (!g->active) {
            g->active = 1;
            {
                managerValue.header = &args;
                managerValue.byte = *((u8 *)&managerValue.header->options);
                g->type = managerValue.byte;
            }
            {
                option.header = &args;
                option.value = *((u8 *)&option.header->count);
                g->count = option.value;
            }
            if (particleValue.priority > 0)
                g->priority = particleValue.priority - 1;
            else
                g->priority = 2;
            i = 0;
            {
                particleValue.priority = args.count;
                if (i < particleValue.priority) {
                    u16 *table = (u16 *)0x0203B400;
                    register u16 *tableReg asm("r7") = table;

                    u8 *sprite;
                    u32 mask;

                    particleValue.particle = pool->particles;
                    sprite = ((u8 *)pool) + 20;
                    mask = 0x3ff;
                    asm("" : "+r"(tableReg), "+r"(g));
                    table = tableReg;
                    i = args.count;

                    do {
                        particleValue.particle->active = 1;
                        particleValue.particle->pad = 1;
                        particleValue.particle->timer = 0;
                        particleValue.particle->lifetime =
                            lifetime + (Random(table, mask, rng) & lifetimeMask);
                        particleValue.particle->cell = CellIndex(args.position);
                        {
                            option.position = args.position;
                            particleValue.particle->height = option.position->y;
                        }
                        if (spread->x) {
                            s32 r = Random(table, mask, rng);
                            particleValue.particle->velocity.x = velocity->x + Mod(r, spread->x);
                        } else
                            particleValue.particle->velocity.x = velocity->x;
                        if (spread->y) {
                            s32 r = Random(table, mask, rng);
                            particleValue.particle->velocity.y = velocity->y + Mod(r, spread->y);
                        } else
                            particleValue.particle->velocity.y = velocity->y;
                        if (spread->z) {
                            s32 r = Random(table, mask, rng);
                            particleValue.particle->velocity.z = velocity->z + Mod(r, spread->z);
                        } else
                            particleValue.particle->velocity.z = velocity->z;
                        if ((Random(table, mask, rng) & 15) <= 7)
                            particleValue.particle->flags |= 4;
                        else
                            particleValue.particle->flags &= ~4;
                        ClearFlag(&particleValue.particle->flags, 1);
                        {
                            option.position = args.position;
                            particleValue.particle->position = *option.position;
                        }
                        {
                            u32 *entries = gUnk_08604B90;
                            s32 type;
                            u32 offset;
                            option.value = args.options;
                            type = option.value;
                            offset = type * sizeof(*entries);
                            particleValue.particle->tile =
                                (*((u16 *)(resource + 4))) + *(u32 *)(offset + (u32)entries);
                        }
                        particleValue.particle->priority = g->priority;
                        sub_08217E40(sprite, 0);
                        particleValue.particle++;
                        sprite += 56;

                    } while (--i);
                    i = args.count;
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
