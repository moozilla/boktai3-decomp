#include "global.h"
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
struct Cell {
    u8 height, tile;
    u16 flags;
};
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
#define gRandomTable ((u16 *)0x0203B400)
extern u32 gUnk_08604B90[];
u8 *TerrainModifiers_FindCellOverride(int, int);
void Effect_Remove(void *);
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
#ifdef NONMATCHING
// fuzzy: 74.5%
// fix.py best (sweep after the inlined-insn fix)
// Register lifetimes and stack offsets still differ in the group and velocity loops.
static inline s32 Random(u16 *table, u32 mask, u32 *rng)
{
    u32 r = (*rng + 1) & mask;
    *rng = r;
    return table[r];
}
static inline void ClearFlag(volatile u32 *p, u32 mask)
{
    u32 v = *p;
    v &= ~mask;
    *p = v;
}
s32 ParticleGroup_SpawnBouncing(s32 count, s32 options, struct Vec *position, struct Vec *velocity,
                                struct Vec *spread, s32 lifetime, s32 lifetimeMask)
{
    struct Manager *m = gParticleGroupManager;
    u8 *resource;
    struct Group *g;
    struct Group *pool;
    u32 *rng;
    s32 priority;
    s32 j;
    register s32 i asm("r5");
    asm("" : "+m"(count), "+m"(options), "+m"(position));
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = options >> 8;
    options &= 255;
    {
        register s32 held asm("r4") = priority;
        asm("" : "+r"(held));
        priority = held;
    }
    if (count > 8)
        count = 8;
    asm("" : "+m"(options));
    if (options <= 20)
        resource = m->small;
    else
        resource = m->large;
    j = 0;
    asm("" : "+r"(rng));
    rng = &gRandomIndex;
    pool = m->groups;
    g = pool;
    asm("" : "+r"(g));
    for (; j < 6; g++, pool++, j++) {
        if (!g->active) {
            g->active = 1;
            g->type = *((u8 *)(&options));
            g->count = *((u8 *)(&count));
            if (priority > 0)
                g->priority = priority - 1;
            else
                g->priority = 2;
            i = 0;
            if (i < count) {
                u16 *table = (u16 *)0x0203B400;
                register u32 mask asm("sb") = 0x3ff;
                struct Particle *p = pool->particles;
                u8 *sprite = ((u8 *)pool) + 20;
                asm("" : "+r"(table), "+r"(mask));
                i = count;
                asm("" : "+r"(i));
                do {
                    p->active = 1;
                    p->pad = 1;
                    p->timer = 0;
                    p->lifetime = lifetime + (Random(table, mask, rng) & lifetimeMask);
                    p->cell = CellIndex(position);
                    p->height = position->y;
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
                        struct Vec *source = position;
                        u32 a = *((u32 *)source);
                        u32 b = *(((u32 *)source) + 1);
                        *((u32 *)(&p->position)) = a;
                        *(((u32 *)(&p->position)) + 1) = b;
                    }
                    p->tile = (*((u16 *)(resource + 4))) + gUnk_08604B90[options];
                    p->priority = g->priority;
                    sub_08217E40(sprite, 0);
                    p++;
                    sprite += 56;
                    asm("" : "+r"(i));
                } while (--i);
                i = count;
            }
            if (i < 8) {
                struct Particle *p = &pool->particles[i];
                do {
                    p->active = 0;
                    p->flags |= 1;
                    p++;
                    i++;
                } while (i < 8);
            }
            break;
        }
    }

    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08014634);
#endif
