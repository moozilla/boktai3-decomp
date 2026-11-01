#ifndef GUARD_SCENE_ACTOR_CARDINAL_PATH_H
#define GUARD_SCENE_ACTOR_CARDINAL_PATH_H

#include "scene_actor_chase_shared.h"
#include "scene_actor_terrain.h"

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);
static inline s32 SceneActorPath_Width(void)
{
    return ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
}
static inline s32 SceneActorPath_Height(void)
{
    return ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->height;
}

static inline u8 SceneActorPath_CanReach(s32 cell, s32 goal, s32 y, u32 kind, s32 total)
{
    s32 i, col;
    if (cell < goal) {
        i = cell + 1;
        for (col = Mod(i, SceneActorPath_Width());
             col < SceneActorPath_Width() && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i++, col++) {
            if (i == goal)
                return 1;
        }
        for (i = cell + SceneActorPath_Width();
             total > i && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i += SceneActorPath_Width()) {
            if (i == goal)
                return 1;
        }
    } else {
        i = cell - 1;
        for (col = Mod(i, SceneActorPath_Width());
             col >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind); i--, col--) {
            if (i == goal)
                return 1;
        }
        for (i = cell - SceneActorPath_Width(); i >= 0 && SceneActorTerrain_CanTraverseCell(cell, i, y, kind);
             i -= SceneActorPath_Width()) {
            if (i == goal)
                return 1;
        }
    }
    return 0;
}

#endif
