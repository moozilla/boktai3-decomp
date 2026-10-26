#define gSceneGridMap    gTerrainMap
#define gSceneGridWidth  gTerrainWidth
#define gSceneGridHeight gTerrainHeight
#include "global.h"
#include "scene_actor_terrain.h"

void SceneGrid_SnapPositionToCellCenter(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    SceneActorTerrain_GetCenter(index, position);
}
