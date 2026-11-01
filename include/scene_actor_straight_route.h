#ifndef GUARD_SCENE_ACTOR_STRAIGHT_ROUTE_H
#define GUARD_SCENE_ACTOR_STRAIGHT_ROUTE_H

#include "global.h"

#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

static inline s32 SceneActorTerrain_Height(void)
{
    return *(s16 *)(gTerrainMap->cells + 6);
}
static inline u32 SceneActorTerrain_Attributes(s32 cell)
{
    return *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
}
static inline u8 SceneActorTerrain_IsLevelOpen(s32 cell, u32 level)
{
    if ((*SceneActorTerrain_GetCell(cell) & 15) != level ||
        (SceneActorTerrain_Attributes(cell) & 0x43))
        return 0;
    return 1;
}
static inline u8 SceneActorTerrain_RequestPath(struct SceneActorRegistryResetView *actor,
                                               void *route, struct ContactPosition *from,
                                               struct ContactPosition *pos)
{

    s32 r;
    if (!Nav_IsPositionInMesh(route, from, pos)) {
        u16 *flags = &actor->flags2D4;
        s32 mask = ~0x200;
        *flags &= mask;
        return 0;
    }
    r = Terrain_ApplyBlocked(route, from, pos);
    if (r) {
        if (r != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    {
        u16 *flags = &actor->flags2D4;
        s32 mask = ~0x200;
        *flags &= mask;
    }
    return 0;
}

static inline s32 SceneActorTerrain_LastOpenCell(struct SceneActorRegistryResetView *actor, s32 dir)
{
    s32 cell, i, col, total;
    u32 level;
    cell = actor->terrainCell;
    level = *SceneActorTerrain_GetCell(cell) & 15;
    total = SceneActorTerrain_Width() * SceneActorTerrain_Height();
    switch (dir) {
        case 0:
            i = cell + 1;
            col = Mod(i, SceneActorTerrain_Width());
            for (; col < SceneActorTerrain_Width(); i++, col++) {
                if (!SceneActorTerrain_IsLevelOpen(i, level)) {
                    return i - 1;
                }
            }
            break;
        case 1:
            for (i = cell + SceneActorTerrain_Width(); total > i; i += SceneActorTerrain_Width()) {
                if (!SceneActorTerrain_IsLevelOpen(i, level)) {
                    return i - SceneActorTerrain_Width();
                }
            }
            break;
        case 2:
            i = cell - 1;
            col = Mod(i, SceneActorTerrain_Width());
            for (; col >= 0; i--, col--) {
                if (!SceneActorTerrain_IsLevelOpen(i, level)) {
                    return i + 1;
                }
            }
            break;
        case 3:
            for (i = cell - SceneActorTerrain_Width(); i >= 0; i -= SceneActorTerrain_Width()) {
                if (!SceneActorTerrain_IsLevelOpen(i, level)) {
                    return i + SceneActorTerrain_Width();
                }
            }
            break;
    }
    return cell;
}

#endif
