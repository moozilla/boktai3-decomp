
#ifdef NONMATCHING
// fuzzy: 93.8%
// foundry draft (worker best 92.6%); what differs is mostly register allocation and instruction order
#define SceneActor_StartAdjacentCellRoute sub_080DA208
#define SceneActor_FindDirectionalCell    sub_080D9B20
#define gCurrentTerrainMap                gUnk_030052F4
#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 sub_0821F204(void *, struct ContactPosition *, struct ContactPosition *);
s32 sub_0821F7A8(void *, struct ContactPosition *, struct ContactPosition *);
s32 sub_080D9B20(void *, s32);
s32 sub_080DA208(struct SceneActorRegistryResetView *actor)
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
    for (; count <= 3; count++) {
        cell = sub_080D9B20(actor, direction);
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
    result = sub_0821F204(route, position, &target);
    if (!result)
        goto clearRoute;
    result = sub_0821F7A8(route, position, &target);
    if (!result)
        goto clearRoute;
    if (result != 2)
        Memory_Clear(&actor->motion, 8);
    goto ready;
clearRoute: {
    u16 *flags = &actor->flags2D4;
    u32 mask = 0xFFFFFDFF;
    u32 value = *flags;
    mask &= value;
    *flags = mask;
}
ready:
    parent->flags6D4 = 30;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DA208);
#endif
