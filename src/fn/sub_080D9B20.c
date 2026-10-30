#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

// confidence=low: retains the current cell unless its requested neighbour passes the height and flag checks.
static inline u32 SceneActor_CellAttributes(s32 cell)
{
    return *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
}

static inline u8 SceneActor_IsCellOpen(s32 cell, s32 level)
{
    if ((*SceneActorTerrain_GetCell(cell) & 15) != level ||
        (SceneActor_CellAttributes(cell) & 0x43))
        return 0;
    return 1;
}
static inline u8 SceneActor_IsAdjacentCellOpen(struct SceneActorRegistryResetView *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 level = actor->body.position.y >> 8;
    s32 i;
    if ((goal - 1) == cell) {
        i = cell + 1;
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
        return 0;
    } else if (goal == (cell + SceneActorTerrain_Width())) {
        i = cell + SceneActorTerrain_Width();
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
        return 0;
    } else if ((goal + 1) == cell) {
        i = cell - 1;
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
        return 0;
    } else if (goal == (cell - SceneActorTerrain_Width())) {
        i = cell - SceneActorTerrain_Width();
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
    }
    return 0;
}

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
