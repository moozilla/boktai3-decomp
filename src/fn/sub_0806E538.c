#include "global.h"
#include "scene_actor_terrain.h"

/* A negative cell is a blocked candidate's encoding; its magnitude is the cell. */
static inline void SceneGrid_GetEndpointCenter(s32 cell, struct ContactPosition *position)
{
    if (cell < 0)
        cell = -cell;
    SceneActorTerrain_GetCenter(cell, position);
}

s32 SceneGrid_ResolveEndpoint(struct ContactPosition *first, struct ContactPosition *second,
                              s32 firstCell, s32 secondCell)
{
    if (secondCell == -255) {
        if (firstCell == secondCell) {
            firstCell = SceneActorTerrain_Width() * 12 + 10;
            SceneGrid_GetEndpointCenter(firstCell, second);
        } else {
            SceneGrid_GetEndpointCenter(firstCell, first);
            return 1;
        }
    } else {
        SceneGrid_GetEndpointCenter(secondCell, first);
    }
    return -1;
}
