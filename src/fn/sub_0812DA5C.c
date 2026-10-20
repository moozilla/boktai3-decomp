#include "global.h"
#include "scene_actor_terrain.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: tests the adjacent cell against the supplied position's height ceiling.
#define SceneActorTerrain_CheckNeighborHeight sub_0812DA5C
u8 SceneActorTerrain_CheckNeighborHeight(s32 direction, s32 cell, struct ContactPosition *position)
{
    struct ContactPosition center;
    u32 ceiling = position->y + 255;
    switch (direction) {
    case 0:
        cell++;
        SceneActorTerrain_GetCenter(cell, &center);
        break;
    case 1:
        cell += SceneActorTerrain_Width();
        SceneActorTerrain_GetCenter(cell, &center);
        break;
    case 2:
        cell--;
        SceneActorTerrain_GetCenter(cell, &center);
        break;
    case 3:
        cell -= SceneActorTerrain_Width();
        SceneActorTerrain_GetCenter(cell, &center);
        break;
    }
    if ((u32)SceneActorTerrain_GetHeight(cell, &center) > ceiling)
        return 0;
    return 1;
}
