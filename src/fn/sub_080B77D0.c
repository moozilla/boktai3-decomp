#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 87.8%
// Remaining differences: cached group/route addresses, north/south scan tails, and terrain-load registers.
s32 Mod(s32, s32);
s32 Div(s32, s32);
u8 sub_0812E118(u32, u32, u32, u32);
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
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
        data = gUnk_030052F4->cells + (cell * 4 + 12);
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
    return *(s16 *)(gUnk_030052F4->cells + 4);
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (sub_0821F204(route, position, destination)) {
        result = sub_0821F7A8(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
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
#define Skeleton_TrySelectRetreatCell sub_080B77D0
static inline u32 SelectRetreat(struct SceneActorRegistryResetView *actor, u32 priorDirection,
                                s32 steps)
{
    struct ContactPosition center;
    s32 tries = 0;
    u32 cell, height, shape;
    s32 chosen, next, i;
    void **route;
    u8 *group = (u8 *)&actor->group;
    do {
        cell = actor->terrainCell;
        height = CellHeight(&actor->position, cell);
        shape = CellShape(cell);
        switch (FacingQuarter(actor->direction + 32) & 3) {
            case 0:
                next = cell + 1;
                i = 0;
                route = &actor->routeObject;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next++;
                    i++;
                } while (i < steps);
                chosen = next - 1;
                break;
            case 1:
                next = cell + MapWidth();
                i = 0;
                route = &actor->routeObject;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next += MapWidth();
                    i++;
                } while (i < steps);
                chosen = next - MapWidth();
                break;
            case 2:
                next = cell - 1;
                i = 0;
                route = &actor->routeObject;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next--;
                    i++;
                } while (i < steps);
                chosen = next + 1;
                break;
            case 3:
                next = cell - MapWidth();
                i = 0;
                route = &actor->routeObject;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next -= MapWidth();
                    i++;
                } while (i < steps);
                chosen = next + MapWidth();
                break;
            default:
                chosen = actor->terrainCell;
                route = &actor->routeObject;
                break;
        }
        if (chosen != actor->terrainCell)
            goto selected;
        actor->direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->direction = priorDirection;
    SceneActorTerrain_GetCenter(chosen, &center);
    {

        if (TryRoute(actor, *route, &actor->position, &center))
            return 1;
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
    return 0;
}

u32 Skeleton_TrySelectRetreatCell(struct SceneActorRegistryResetView *actor, u32 priorDirection)
{
    return SelectRetreat(actor, priorDirection, 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080B77D0);
#endif
