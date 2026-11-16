#include "global.h"
#include "engine.h"
#include "particle_group.h"
#include "terrain_collision.h"
extern u32 gUnk_08604B90[];
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline s32 FrozenGroup_LoadType(struct ParticleGroup *g)
{
    s32 type = g->type;
    if (type < 0)
        type = -1;
    return type;
}

void ParticleGroup_UpdateFrozen(struct ParticleGroupManager *m, struct ParticleGroup *g)
{
    struct ParticleGroupParticle *p;
    u32 base;
    s32 n;
    u32 type;
    if (g->type <= 8)
        base = gUnk_08604B90[0x27];
    else
        base = gUnk_08604B90[g->type];
    p = g->particles;
    n = 7;
    do {
        struct ParticleGroupGraphics *e;
        u32 cell, v;
        u8 *q;
        u32 fl;
        type = FrozenGroup_LoadType(g);
        if (type <= 0x14)
            e = (struct ParticleGroupGraphics *)m->small;
        else
            e = (struct ParticleGroupGraphics *)m->large;
        if (!(p->flags & 1)) {
            s32 x = (s16)(u16)p->position.x >> 8;
            s32 y = (s16)(u16)p->position.z >> 8;
            u32 t;
            if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
                t = 0xFFFF;
            else
                t = gTerrainMap->rows[y] + x;
            cell = (u16)t;
            if (cell != 0xFFFF) {
                q = TerrainModifiers_FindCellOverride(cell, 1);
                if (q)
                    q = (u8 *)&((struct TerrainCellOverride *)q)->cell;
                else
                    q = (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
                if ((fl = ((struct TerrainCell *)q)->flags) & 0x20) {
                    v = 0xFFFF;
                    type = g->type;
                } else {
                    q = TerrainModifiers_FindCellOverride(cell, 1);
                    if (q)
                        q = (u8 *)&((struct TerrainCellOverride *)q)->cell;
                    else
                        q = (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
                    v = (((struct TerrainCell *)q)->shapeAndHeight & 0xf) << 8;
                    type = g->type;
                }
            } else
                v = cell;
            if ((u8)type <= 8 && (m->timer & 1) && v != 0xFFFF) {
                p->position.y = v;
                p->tile = e->tileBase + base;
            } else {
                p->position.y = (u16)p->height;
                p->tile = e->tileBase + *(u32 *)((g->type << 2) + (u32)gUnk_08604B90);
            }
        }
        p++;
        n--;
    } while (n >= 0);
}
