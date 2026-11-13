#ifndef GUARD_TERRAIN_SHADOW_H
#define GUARD_TERRAIN_SHADOW_H

#include "engine.h"
#include "scene_vec.h"

struct ShadowTileResource {
    u16 unknown0;
    u8 flags, shape;
    u16 tileBase, paletteBank;
};

/* Tile drawable shared by the terrain and scaled-shadow owners. */
struct ShadowTileSprite {
    u32 flags;
    u8 queued, scaleX, scaleY, rotation;
    u16 tile;
    u8 width, height;
    s8 originX, originY;
    u8 palette, mode, status, list;
    u16 unknown12;
    u32 attributes;
    struct SceneVec position;
    struct ShadowTileSprite *previous, *next;
};

struct TerrainShadow {
    u8 active, flags;
    u16 unknown2, unknown4, unknown6;
    struct SceneVec *source;
    struct ShadowTileSprite sprite;
    void (*update)(struct TerrainShadow *);
    struct TerrainShadow *previous, *next;
};

struct TerrainShadowManager {
    struct Task task;
    struct ShadowTileResource *resource;
    struct TerrainShadow *head;
};

s32 ShadowList_Insert(struct TerrainShadowManager *, struct TerrainShadow *);
s32 ShadowList_Remove(struct TerrainShadowManager *, struct TerrainShadow *);
void ActorShadow_UpdateNone(struct TerrainShadow *);
void Shadow_UpdateTerrain(struct TerrainShadow *);
u32 ShadowTask_Update(struct TerrainShadowManager *);
void ShadowTask_Destroy(void);
s32 ShadowTask_Init(struct TerrainShadowManager *, void *);
struct TerrainShadowManager *ShadowTask_Create(void *, s32);
s32 Shadow_Initialize(struct TerrainShadow *, struct SceneVec *, s32);
u32 Shadow_Destroy(struct TerrainShadow *);
void ScaledShadowTask_Destroy(void);

void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);

struct ShadowTerrainCell {
    u8 height, tile;
    u16 flags;
};
struct ShadowTerrainOverride {
    u16 id;
    u8 active, kind;
    struct ShadowTerrainCell cell;
};
struct ShadowTerrainGrid {
    u32 unknown0;
    u16 width, height;
    s16 originX, originY;
    struct ShadowTerrainCell cells[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline int ShadowTerrain_CellIndex(struct SceneVec *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline struct ShadowTerrainCell *ShadowTerrain_Cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return &((struct ShadowTerrainOverride *)p)->cell;
    return &((struct ShadowTerrainGrid *)gTerrainMap->cells)->cells[i];
}
static inline int ShadowTerrain_HidesShadow(struct ShadowTerrainCell *c)
{
    u32 value = c->flags;
    value &= 32;
    return value;
}

#endif
