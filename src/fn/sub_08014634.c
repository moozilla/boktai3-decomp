#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "engine.h"
#include "ram.h"
// confidence=low: fills the first inactive group with randomized bouncing particles.
#define ParticleGroup_SpawnBouncing sub_08014634
struct Vec {
    s16 x, y, z, pad;
};
struct Particle {
    u8 active, pad, timer, lifetime;
    u16 cell;
    s16 height;
    struct Vec velocity;
    volatile u32 flags;
    u8 pad14[4];
    u16 tile;
    u8 pad1a[5], priority, pad20[8];
    struct Vec position;
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
static inline int CellIndex(struct Vec *p)
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
    struct Vec *position;
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
s32 ParticleGroup_SpawnBouncing(s32 count, s32 options, struct Vec *position,
                                struct Vec *velocityArg, struct Vec *spread, s32 lifetime,
                                s32 lifetimeMask)
{
    struct SpawnArgs args = {count, options, position};
    struct Vec *velocityInput = velocityArg;
    struct Vec *velocity;
    register struct Manager *m asm("r1");
    u8 *resource;
    struct Group *g;
    struct Group *pool;
    u32 *rng;
    s32 priority;
    register s32 optionMask asm("r0");
    s32 j;
    register s32 i asm("r5");

    velocity = velocityInput;
    m = gParticleGroupManager;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = args.options >> 8;
    asm("" : "+m"(args.options));
    optionMask = 255;
    args.options &= optionMask;
    if (args.count > 8) {
        register s32 limit asm("r2") = 8;
        args.count = limit;
    }

    if (args.options <= 20) {
        register u8 *small asm("r2") = m->small;
        asm("" : "+r"(small));
        resource = small;
    } else {
        register u8 *large asm("r0") = m->large;
        asm("" : "+r"(large));
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
                register struct SpawnArgs *header asm("r1") = &args;
                register u32 byte asm("r1") = *((u8 *)&header->options);
                g->type = byte;
            }
            {
                register struct SpawnArgs *header asm("r2") = &args;
                register u32 byte asm("r2") = *((u8 *)&header->count);
                g->count = byte;
            }
            if (priority > 0)
                g->priority = priority - 1;
            else
                g->priority = 2;
            i = 0;
            {
                register s32 bound asm("r4") = args.count;
                if (i < bound) {
                    u16 *table = (u16 *)0x0203B400;
                    register u16 *tableReg asm("r7") = table;
                    struct Particle *p = pool->particles;
                    u8 *sprite = ((u8 *)pool) + 20;
                    u32 mask = 0x3ff;

                    asm("" : "+r"(tableReg), "+r"(g));
                    table = tableReg;
                    i = args.count;

                    do {
                        p->active = 1;
                        p->pad = 1;
                        p->timer = 0;
                        p->lifetime = lifetime + (Random(table, mask, rng) & lifetimeMask);
                        p->cell = CellIndex(args.position);
                        {
                            register struct Vec *source asm("r2") = args.position;
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
                            register struct Vec *source asm("r2") = args.position;
                            p->position = *source;
                        }
                        {
                            register u32 *entries asm("r1") = gUnk_08604B90;
                            s32 type;
                            u32 offset;
                            type = args.options;
                            offset = type * sizeof(*entries);
                            asm("" : "+r"(offset) : "r"(type));
                            p->tile = (*((u16 *)(resource + 4))) + *(u32 *)(offset + (u32)entries);
                        }
                        p->priority = g->priority;
                        sub_08217E40(sprite, 0);
                        p++;
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
