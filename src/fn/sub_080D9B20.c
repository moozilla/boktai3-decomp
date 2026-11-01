#include "scene_actor_adjacent_cell.h"

// confidence=low: retains the current cell unless its requested neighbour passes the height and flag checks.
static inline s32 SceneActor_NextRowCell(const u16 *cell)
{
    s32 width = SceneActorTerrain_Width();
    return *cell + width;
}

static inline s32 SceneActor_PreviousRowCell(s32 cell)
{
    return cell - SceneActorTerrain_Width();
}

s32 SceneActor_FindDirectionalCell(struct SceneActorRegistryResetView *actor, s32 direction)
{
    s32 result = actor->terrainCell;
    switch (direction) {
        case 0:
            if (SceneActor_IsAdjacentCellOpen(actor, result + 1))
                result = actor->terrainCell + 1;
            break;

        case 1:
            if (SceneActor_IsAdjacentCellOpen(actor, result + SceneActorTerrain_Width()))
                result = SceneActor_NextRowCell(&actor->terrainCell);
            break;

        case 2:
            if (SceneActor_IsAdjacentCellOpen(actor, result - 1))
                result = actor->terrainCell - 1;
            break;

        case 3:
            if (SceneActor_IsAdjacentCellOpen(actor, result - SceneActorTerrain_Width()))
                result = SceneActor_PreviousRowCell(actor->terrainCell);
            break;
    }

    return result;
}
