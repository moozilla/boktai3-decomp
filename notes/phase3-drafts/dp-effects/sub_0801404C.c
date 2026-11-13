#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
// confidence=low: integrates group particles and bounces them against terrain.
#define ParticleGroup_UpdateBouncing sub_0801404C
struct Vec {
    s16 x, y, z, pad;
};
struct Particle {
    u8 active, pad, timer, lifetime;
    u16 cell;
    s16 height;
    struct Vec velocity;
    u32 flags;
    u8 pad14[4];
    u16 tile;
    u8 pad1a[5], priority, pad20[8];
    struct Vec position;
    u8 tail[8];
};
struct Group {
    u8 active, count, type, priority;
    struct Particle particles[1];
};
struct ParticleGraphics {
    u32 reserved;
    u16 tileBase;
};
struct Manager {
    u8 pad[24];
    u32 timer;
    struct ParticleGraphics *small, *large;
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
extern u32 gUnk_08604B90[];
u8 *TerrainModifiers_FindCellOverride(int, int);
void Effect_Remove(void *);
static inline u32 CoordinateBits(const s16 *coordinate)
{
    return (u16)*coordinate;
}
static inline int CellIndex(struct Vec *p)
{
    int x = (s16)CoordinateBits(&p->x) >> 8;
    int z = (s16)CoordinateBits(&p->z) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0xffff;
    return gTerrainMap->rows[z] + x;
}
static inline struct Cell *Cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return (void *)(p + 4);
    return (void *)(gTerrainMap->cells + 12 + i * 4);
}
static inline int Scale(int a, int b)
{
    int v = a * b;
    if (v >= 0)
        return v >> 6;
    return -(-v >> 6);
}
void ParticleGroup_UpdateBouncing(struct Manager *m, struct Group *g)
{
    int i;
    int alive;
    u32 base = gUnk_08604B90[0x27];
    struct Particle *p;
    u8 *sprite;
    alive = 0;
    i = 0;
    if (i < g->count) {
        u16 *table = gRandomTable;
        p = g->particles;
        sprite = (u8 *)g + 20;
        do {
            if (p->active) {
                u16 index = CellIndex(&p->position);
                struct ParticleGraphics *resource;
                int height;
                if (g->type <= 20)
                    resource = m->small;
                else
                    resource = m->large;
                p->position.x += p->velocity.x;
                p->position.z += p->velocity.z;
                p->height += p->velocity.y;
                p->velocity.y--;
                if (index != 0xffff) {
                    height = (Cell(index)->height & 15) << 8;
                    if (p->height < height) {
                        if (index != p->cell) {
                            int d = index - p->cell;
                            if (d < 0)
                                d = -d;
                            if (d == 1) {
                                u32 random = ParticleRandom_ReadSample(table);
                                p->velocity.x = -Scale(p->velocity.x, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.x = (p->position.x & ~255) + 256;
                                else
                                    p->position.x = (p->position.x & ~255) - 1;
                            } else {
                                u32 random = ParticleRandom_ReadSample(table);
                                p->velocity.z = -Scale(p->velocity.z, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.z = (p->position.z & ~255) + 256;
                                else
                                    p->position.z = (p->position.z & ~255) - 1;
                            }
                        } else
                            p->height = height;
                        {
                            u32 random = ParticleRandom_ReadSample(table);
                            p->velocity.y = -Scale(p->velocity.y, (random & 3) + 55);
                        }
                    }
                    {
                        u32 flags = Cell(index)->flags;
                        u32 mask = 32;
                        flags &= mask;
                        if (flags)
                            height = 0xffff;
                    }
                } else
                    height = 0xffff;
                p->cell = index;
                if ((m->timer & 1) == (i & 1)) {
                    if (height != 0xffff) {
                    p->position.y = height;
                    p->tile = resource->tileBase + base;
                    p->priority = 3;
                        goto age;
                    }
                    p->position.y = p->height;
                    p->tile = resource->tileBase + gUnk_08604B90[g->type];
                    p->priority = g->priority;
                    goto age;
                }
                    p->position.y = p->height;
                    p->tile = resource->tileBase + gUnk_08604B90[g->type];
                    p->priority = g->priority;
            age:
                if (p->timer++ >= p->lifetime) {
                    p->active = 0;
                    Effect_Remove(sprite);
                } else
                    alive++;
            }
            p++;
            sprite += 56;
            i++;
        } while (i < g->count);
    }
    if (!alive)
        g->active = 0;
}
