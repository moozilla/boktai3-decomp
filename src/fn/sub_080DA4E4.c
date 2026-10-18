#define gSceneGridMap                      gUnk_030052F4
#define gSceneGridWidth                    gUnk_030052F8
#define gSceneGridHeight                   gUnk_030052FC
#include "global.h"
#include "scene_actor_terrain.h"

void SceneGrid_SnapPositionToCellCenter(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    SceneActorTerrain_GetCenter(index, position);
}
