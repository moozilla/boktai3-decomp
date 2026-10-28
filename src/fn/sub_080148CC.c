#include "global.h"
#include "engine_globals.h"
// confidence=low: fills a particle group with randomized polar velocities.
#define ParticleGroup_SpawnRadial sub_080148CC
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
    u8 pad1a[4], palette, priority, pad20[8];
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
    return ((struct Map *)gTerrainMap)->rows[z] + x;
}
extern struct Manager *gUnk_02000060;
struct Manager *ParticleManager_Create(void *, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, s32);
#ifdef NONMATCHING
// fuzzy: 80.0%
// fix.py best (sweep after the inlined-insn fix)
// Loop pointer lifetimes, stack allocation, and some inline arithmetic still differ.
static inline s32 Random(u16 *table, u32 *rng)
{
    u32 r = (*rng + 1) & 0x3ff;
    *rng = r;
    return table[r];
}
static inline void ClearFlag(volatile u32 *p, u32 mask)
{
    u32 v = *p;
    v &= ~mask;
    *p = v;
}
extern s16 gUnk_086149C4[];
static inline s32 FixedMultiply(s32 a, s32 b)
{
    s32 value = a * b;
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Direction(struct Vec *out, s32 angle, s32 speed)
{
    out->x = FixedMultiply(gUnk_086149C4[(angle + 64) & 255], speed);
    out->y = 0;
    out->z = FixedMultiply(gUnk_086149C4[angle & 255], speed);
}
s32 ParticleGroup_SpawnRadial(s32 count, s32 options, struct Vec *position, s32 angle,
                              s32 angleRange, s32 speed, s32 speedRange, s32 vertical,
                              s32 verticalRange, s32 lifetime, s32 lifetimeMask)
{
    struct Manager *m = gUnk_02000060;
    struct Group *g;
    struct Group *pool;
    u32 *rng;
    s32 priority;
    s32 j;
    s32 halfRange;
    s32 palette;
    s32 angleOffset;
    s32 i;
    u8 *resource;
    typeof(i) c1;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = options >> 8;
    options &= 255;
    if (count > 8)
        count = 8;
    if (options <= 20) {
        resource = m->small;
        palette = *((u16 *)(resource + 6));
    } else {
        resource = m->large;
        palette = (*((u16 *)(resource + 6))) + 1;
    }
    halfRange = angleRange >> 1;
    j = 0;
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
            else {
                g->priority = 2;
            }
            i = 0;
            if (i < count) {
                u16 *table = (u16 *)0x0203B400;
                register struct Vec *vector asm("sb") = (void *)(((u8 *)pool) + 12);
                struct Particle *p;
                u8 *sprite;
                asm("" : "+r"(vector));
                p = pool->particles;
                sprite = ((u8 *)pool) + 20;
                asm("" : "+r"(table));
                angleOffset = (angle & 255) * 2;
                i = count;
                c1 = i;
                asm("" : "+r"(c1));
                do {
                    p->active = 1;
                    p->pad = 1;
                    p->timer = 0;
                    p->lifetime = lifetime + (Random(table, rng) & lifetimeMask);
                    p->cell = CellIndex(position);
                    p->height = position->y;
                    {
                        s32 magnitude;
                        if (speedRange) {
                            s32 r = Random(table, rng);
                            magnitude = speed + Mod(r, speedRange);
                        } else
                            magnitude = speed;
                        if (angleRange) {
                            s32 r = Random(table, rng);
                            s32 offset = Mod(r, angleRange);
                            Direction(vector, (angle - halfRange) + offset, magnitude);
                        } else {
                            struct Vec *out = vector;
                            asm("" : "+r"(out));
                            out->x = FixedMultiply(gUnk_086149C4[(angle + 64) & 255], magnitude);
                            out->y = 0;
                            out->z = FixedMultiply(*((s16 *)(((u8 *)gUnk_086149C4) + angleOffset)),
                                                   magnitude);
                        }
                    }
                    if (verticalRange) {
                        s32 r = Random(table, rng);
                        p->velocity.y = vertical + Mod(r, verticalRange);
                    } else
                        p->velocity.y = vertical;
                    if ((Random(table, rng) & 15) <= 7)
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
                    p->palette = palette;
                    p->tile = (*((u16 *)(resource + 4))) + gUnk_08604B90[options];
                    p->priority = g->priority;
                    sub_08217E40(sprite, 0);
                    vector++;
                    vector = (void *)(((u8 *)vector) + 48);
                    asm("" : "+r"(vector));
                    p++;
                    sprite += 56;
                    asm("" : "+r"(c1));
                } while (--c1);
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
INCLUDE_ASM("asm/nonmatching", sub_080148CC);
#endif
