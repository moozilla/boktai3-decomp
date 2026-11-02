#ifndef GUARD_SCENE_ACTOR_ADJACENT_CELL_H
#define GUARD_SCENE_ACTOR_ADJACENT_CELL_H

#include "scene_actor_chase_types.h"
#include "scene_actor_terrain.h"

static inline u32 SceneActor_CellAttributes(s32 cell)
{
    const struct SceneActorChaseTerrainCell *entry = (void *)SceneActorTerrain_GetCell(cell);
    return entry->flags;
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
    s32 i, width;
    if ((goal - 1) == cell) {
        i = cell + 1;
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
        return 0;
    }
    width = SceneActorTerrain_Width();
    if (goal == (cell + width)) {
        i = cell + width;
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
    }
    width = SceneActorTerrain_Width();
    if (goal == (cell - width)) {
        i = cell - width;
        if (!SceneActor_IsCellOpen(i, level))
            return 0;
        if (i == goal)
            return 1;
    }
    return 0;
}

#endif
