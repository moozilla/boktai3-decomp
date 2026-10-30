#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
// confidence=low: neighboring Skeleton controls establish provisional role
// MATCHING_CONCESSION: register-pinning
// Keep the map address live through each final signed width load.
s32 Mod(s32, s32);
s32 Div(s32, s32);
u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

static inline u32 CellShape(u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    return *data >> 4;
}
static inline s32 FacingQuarter(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}
static inline s32 MapWidth(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (Nav_IsPositionInMesh(route, position, destination)) {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        }
    }
    {
        u16 *flags = &actor->flags2D4;
        u32 mask = ~0x200;
        mask &= *flags;
        *flags = mask;
    }
    return 0;
}
// confidence=low: probes neighboring cells in facing order and routes toward a retreat cell
static inline u32 SelectRetreat(struct SceneActorRegistryResetView *actor, u32 priorDirection,
                                s32 steps)
{
    struct ContactPosition center;
    s32 tries = 0;
    u32 cell, height, shape;
    s32 chosen, next, i;
    void **route;
    struct EngineMap **mapSlot;

    do {
        cell = actor->terrainCell;
        height = CellHeight(&actor->body.position, cell);
        shape = CellShape(cell);
        switch (FacingQuarter(actor->body.direction + 32) & 3) {
            case 0:
                next = cell + 1;
                i = 0;
                route = &actor->group.route;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                    i++;
                } while (i < steps);
                chosen = next - 1;
                break;
            case 1:
                next = cell + MapWidth();
                i = 0;
                route = &actor->group.route;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto finishSouth;
                    mapSlot = &gTerrainMap;
                    next += *(s16 *)((*mapSlot)->cells + 4);
                    i++;
                } while (i < steps);
                goto finishSouthLoaded;
            case 2:
                next = cell - 1;
                i = 0;
                route = &actor->group.route;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                    i++;
                } while (i < steps);
                chosen = next + 1;
                break;
            case 3:
                next = cell - MapWidth();
                i = 0;
                route = &actor->group.route;
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto finishNorth;
                    mapSlot = &gTerrainMap;
                    next -= *(s16 *)((*mapSlot)->cells + 4);
                    i++;
                } while (i < steps);
                goto finishNorthLoaded;
            finishSouth:
                mapSlot = &gTerrainMap;
            finishSouthLoaded:
                chosen = next - *(s16 *)((*mapSlot)->cells + 4);
                asm volatile("" : "+r"(mapSlot));
                break;
            finishNorth:
                mapSlot = &gTerrainMap;
            finishNorthLoaded:
                chosen = next + *(s16 *)((*mapSlot)->cells + 4);
                asm volatile("" : "+r"(mapSlot));
                break;
            default:
                chosen = actor->terrainCell;
                route = &actor->group.route;
                break;
        }
        if (chosen != actor->terrainCell)
            goto selected;
        actor->body.direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->body.direction = priorDirection;
    SceneActorTerrain_GetCenter(chosen, &center);
    {
        s16 groupOffset = (u32) & ((struct SceneActorRegistryResetView *)0)->group.common;
        u8 *group = (u8 *)actor + groupOffset;
        if (TryRoute(actor, *route, &actor->body.position, &center))
            return 1;
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
    return 0;
}

u32 Skeleton_TrySelectRetreatCell(struct SceneActorRegistryResetView *actor, u32 priorDirection)
{
    return SelectRetreat(actor, priorDirection, 1);
}
