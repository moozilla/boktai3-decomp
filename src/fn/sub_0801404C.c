#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
#include "particle_group.h"
#include "terrain_shadow.h"
// confidence=low: integrates group particles and bounces them against terrain.
#define ParticleGroup_UpdateBouncing sub_0801404C
extern u32 gUnk_08604B90[];
void Effect_Remove(void *);
static inline u32 CoordinateBits(const s16 *coordinate)
{
    return (u16)*coordinate;
}
static inline int CellIndex(struct ContactPosition *p)
{
    int x = (s16)CoordinateBits(&p->x) >> 8;
    int z = (s16)CoordinateBits(&p->z) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0xffff;
    return gTerrainMap->rows[z] + x;
}
static inline int Scale(int a, int b)
{
    int v = a * b;
    if (v >= 0)
        return v >> 6;
    return -(-v >> 6);
}
void ParticleGroup_UpdateBouncing(struct ParticleGroupManager *m, struct ParticleGroup *g)
{
    int i;
    int alive;
    u32 base = gUnk_08604B90[0x27];
    struct ParticleGroupParticle *p;
    u8 *sprite;
    alive = 0;
    i = 0;
    if (i < g->count) {
        union ParticleRandomTableSource source;
        const u16 *table;
        source.halves[0] = (u16)RANDOM_TABLE_ADDR;
        source.halves[1] = RANDOM_TABLE_ADDR >> 16;
        table = ParticleRandom_GetTable(&source);
        p = g->particles;
        sprite = (u8 *)&g->particles[0].flags;
        do {
            if (p->active) {
                u16 index = CellIndex(&p->position);
                struct ParticleGroupGraphics *resource;
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
                    height = (ShadowTerrain_Cell(index)->height & 15) << 8;
                    if (p->height < height) {
                        if (index != p->cell) {
                            int d = index - p->cell;
                            if (d < 0)
                                d = -d;
                            if (d == 1) {
                                u32 random = ParticleRandom_ReadDirectSample(table);
                                p->velocity.x = -Scale(p->velocity.x, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.x = (p->position.x & ~255) + 256;
                                else
                                    p->position.x = (p->position.x & ~255) - 1;
                            } else {
                                u32 random = ParticleRandom_ReadDirectSample(table);
                                p->velocity.z = -Scale(p->velocity.z, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.z = (p->position.z & ~255) + 256;
                                else
                                    p->position.z = (p->position.z & ~255) - 1;
                            }
                        } else
                            p->height = height;
                        {
                            u32 random = ParticleRandom_ReadDirectSample(table);
                            p->velocity.y = -Scale(p->velocity.y, (random & 3) + 55);
                        }
                    }
                    if (ShadowTerrain_HidesShadow(ShadowTerrain_Cell(index)))
                        height = 0xffff;
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
            sprite += sizeof(*p);
            i++;
        } while (i < g->count);
    }
    if (!alive)
        g->active = 0;
}
