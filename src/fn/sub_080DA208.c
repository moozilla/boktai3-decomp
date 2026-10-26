#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"

#define gCurrentTerrainMap gTerrainMap
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
s32 SceneActor_FindDirectionalCell(void *, s32);
s32 SceneActor_StartAdjacentCellRoute(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *parent = actor->context;
    u8 *group = (void *)&actor->group;
    s32 direction = (((actor->direction + 0x20) >> 6) + 2) & 3;
    s32 cell = 0;
    s32 count = 0;
    struct ContactPosition *targetPointer;
    s32 cellX, cellZ;
    u8 *terrain;
    void *route;
    struct ContactPosition *position;
    s32 result;
    u16 *flagsPointer;
    for (; count <= 3; count++) {
        cell = SceneActor_FindDirectionalCell(actor, direction);
        direction = (direction + 1) & 3;
        if (cell != actor->terrainCell)
            break;
    }
    targetPointer = &target;
    cellX = Mod(cell, *(s16 *)(gCurrentTerrainMap->cells + 4));
    cellZ = Div(cell, *(s16 *)(gCurrentTerrainMap->cells + 4));
    target.x = (cellX << 8) + 0x80;
    terrain = TerrainModifiers_FindCellOverride(cell, 1);
    if (terrain)
        terrain += 4;
    else
        terrain = gCurrentTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*terrain & 15) << 8;
    targetPointer->z = (cellZ << 8) + 0x80;
    target.y = actor->position.y;
    route = *(void **)(group + 0x1FC);
    position = &actor->position;
    result = Nav_IsPositionInMesh(route, position, &target);
    if (!result) {
        u16 *flags = &actor->flags2D4;
        u32 mask = 0xFFFFFDFF;
        u32 value = *flags;
        mask &= value;
        *flags = mask;
    } else {
        result = Terrain_ApplyBlocked(route, position, &target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
        } else {
            u16 *flags = &actor->flags2D4;
            u32 mask = 0xFFFFFDFF;
            u32 value = *flags;
            mask &= value;
            *flags = mask;
        }
    }
    parent->flags6D4 = 30;
    return 1;
}
