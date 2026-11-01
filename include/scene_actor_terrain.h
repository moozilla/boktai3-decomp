#ifndef GUARD_SCENE_ACTOR_TERRAIN_H
#define GUARD_SCENE_ACTOR_TERRAIN_H
#include "engine.h"
#include "contact_actor.h"
/* Narrow terrain/query phase of the registered actor. */
struct SceneActorTerrainView {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[2];
    struct ContactPosition position;
    u8 unknown64[0x190];
    u32 scriptParameterI;
    u32 scriptParameterD;
    u32 scriptParametersG[5];
    u8 unknown210[0xE4];
    u16 terrainCell;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 SceneActorTerrain_Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline u8 *SceneActorTerrain_GetCell(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline void SceneActorTerrain_GetCenter(s32 cell, struct ContactPosition *position)
{
    s32 x = Mod(cell, SceneActorTerrain_Width());
    s32 z = Div(cell, SceneActorTerrain_Width());
    position->x = ((u32)x << 8) + 128;
    position->y = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}
static inline s32 SceneActorTerrain_GetHeight(s32 cell, const struct ContactPosition *position)
{
    u8 *attributes = SceneActorTerrain_GetCell(cell);
    u32 kind = *attributes >> 4;
    s32 height = (*attributes & 15) << 8;
    switch (kind) {
        case 1:
            height -= ((const u8 *)position)[4];
            break;
        case 2:
            height -= ((const u8 *)position)[0];
            break;
    }
    return height;
}
static inline s32 SceneActorTerrain_GetCellIndex(const struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline void SceneActorTerrain_Subtract(struct ContactPosition *out,
                                              const struct ContactPosition *a,
                                              const struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
/* Collision-control prefix used by the adjacent reset/disable helpers. */
struct SceneActorTerrainCollisionView {
    void *object, *drawable;
    u8 drawableKind;
    u8 unknown9[0x8F];
    u8 category;
    u8 unknown99[0x9B];
    struct ContactCollider collider;
    u8 unknown188[0x144];
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u8 unknown2D8[0xC];
    u16 field2E4, field2E6;
};
void SceneActorTerrain_ClearCollisionTimers(struct SceneActorTerrainCollisionView *);
#endif
