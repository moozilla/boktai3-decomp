#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.3%
// The compiler merges the repeated passability tests and removes redundant cell comparisons.
// fuzzy: 85.3%
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
// confidence=low: keeps the current cell unless the requested neighbour passes the height and flag tests.
static inline u8 Open(s32 cell, s32 level)
{
    if ((*SceneActorTerrain_GetCell(cell) & 15) != level ||
        (*(u16 *)(SceneActorTerrain_GetCell(cell) + 2) & 0x43))
        return 0;
    return 1;
}
static inline u8 Adjacent(struct SceneActorRegistryResetView *actor, s32 goal)
{
    s32 cell = actor->terrainCell;
    s32 level = actor->position.y >> 8;
    s32 i;
    if (goal - 1 == cell) {
        i = cell + 1;
        if (!Open(i, level))
            return 0;
        if (i == goal)
            return 1;
    } else if (goal == cell + SceneActorTerrain_Width()) {
        i = cell + SceneActorTerrain_Width();
        if (!Open(i, level))
            return 0;
        if (i == goal)
            return 1;
    } else if (goal + 1 == cell) {
        i = cell - 1;
        if (!Open(i, level))
            return 0;
        if (i == goal)
            return 1;
    } else if (goal == cell - SceneActorTerrain_Width()) {
        i = cell - SceneActorTerrain_Width();
        if (!Open(i, level))
            return 0;
        if (i == goal)
            return 1;
    }
    return 0;
}
s32 SceneActor_FindDirectionalCell(struct SceneActorRegistryResetView *actor, s32 direction)
{
    s32 result = actor->terrainCell;
    switch (direction) {
        case 0:
            if (Adjacent(actor, result + 1))
                result = actor->terrainCell + 1;
            break;
        case 1:
            if (Adjacent(actor, result + SceneActorTerrain_Width()))
                result = actor->terrainCell + SceneActorTerrain_Width();
            break;
        case 2:
            if (Adjacent(actor, result - 1))
                result = actor->terrainCell - 1;
            break;
        case 3:
            if (Adjacent(actor, result - SceneActorTerrain_Width()))
                result = actor->terrainCell - SceneActorTerrain_Width();
            break;
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActor_FindDirectionalCell);
#endif
