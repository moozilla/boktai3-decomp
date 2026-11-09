#include "global.h"
#include "scene_actor_terrain.h"
/* SceneGrid_TryRandomNearbyPosition: Samples one nearby grid position and writes its cell center only if the board entry matches the source terrain height */
#define SceneGrid_TryRandomNearbyPosition sub_0806E34C
extern const s8 gUnk_08605BC8[];

s32 SceneGrid_TryRandomNearbyPosition(struct ContactPosition *output, void *unused,
                                      const struct ContactPosition *origin, s32 radius)
{
    s32 cell = SceneActorTerrain_GetCellIndex(origin);
    s32 column = Mod(cell, SceneActorTerrain_Width());
    s32 row = Div(cell, SceneActorTerrain_Width());
    s32 height = *SceneActorTerrain_GetCell(cell) & 15;
    s32 left = column - radius;
    s32 top = row - radius;
    s32 span = radius * 2 + 1;
    const u16 *random = gRandomTable;
    s32 x, z, offset;
    const s8 *board;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    x = Mod(random[gRandomIndex], span) + left;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    z = Mod(random[gRandomIndex], span) + top;
    board = gUnk_08605BC8;
    offset = z * 8 - 69;
    if (board[x + offset] == height) {
        cell = x + z * SceneActorTerrain_Width();
        SceneActorTerrain_GetCenter(cell, output);
        return 1;
    }
    return 0;
}
