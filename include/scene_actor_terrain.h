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
u8 *sub_0821D64C(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 SceneActorTerrain_Width(void)
{
    return *(s16 *)(gUnk_030052F4->cells + 4);
}
static inline u8 *SceneActorTerrain_GetCell(s32 cell)
{
    u8 *override = sub_0821D64C(cell, 1);
    if (override)
        return override + 4;
    return gUnk_030052F4->cells + (cell * 4 + 12);
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
    register u32 attributes asm("r1") = *SceneActorTerrain_GetCell(cell);
    u32 kind = attributes >> 4;
    s32 height = (attributes & 15) << 8;
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
#endif
