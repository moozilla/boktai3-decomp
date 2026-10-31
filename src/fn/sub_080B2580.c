#define SCENE_ACTOR_CHASE_JOIN_ROUTE_FAILURES 1
#include "scene_actor_chase_reaction.h"

// Tentative: searches progressively shorter cardinal routes around the actor.
u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);
static inline s32 Vorn_RoundDirection(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -((-n) >> 6);
}
static inline s32 Vorn_MapSlotWidth(struct EngineMap **slot)
{
    return ((struct SceneActorChaseTerrainGrid *)(*slot)->cells)->width;
}
static inline s32 Vorn_SweepEscapeCells(struct SceneActorRegistryResetView *actor, s32 range)
{
    struct EngineMap **cachedMapSlot = &gTerrainMap;
    s32 cell = actor->terrainCell;
    u32 height = SceneActorChase_HeightAt(cell, &actor->body.position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 direction = Vorn_RoundDirection(actor->body.direction + 32) & 3;
    s32 next;
    s32 i;
    struct EngineMap **completionMapSlot;

    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < range);
            goto east;
        case 1:
            next = cell + SceneActorChase_MapWidth(*cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto south;
                    next += SceneActorChase_MapWidth(*cachedMapSlot);
                } while (++i < range);
            return next - Vorn_MapSlotWidth(completionMapSlot);
        case 2:
            next = cell - 1;
            i = 0;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < range);
            goto west;
        case 3:
            next = cell - SceneActorChase_MapWidth(*cachedMapSlot);
            i = 0;
            completionMapSlot = (struct EngineMap **)0x030052F4;
            if (i < range)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto north;
                    next -= SceneActorChase_MapWidth(*cachedMapSlot);
                } while (++i < range);
            return next + Vorn_MapSlotWidth(completionMapSlot);
        default:
            goto home;
    }
east:
    return next - 1;
south:
    return next - Vorn_MapSlotWidth(cachedMapSlot);
west:
    return next + 1;
north:
    return next + Vorn_MapSlotWidth(cachedMapSlot);
home:
    return actor->terrainCell;
}

static inline u32 Vorn_HeightFromAttributes(u32 value)
{
    u32 height = 15;
    height &= value;
    return height << 8;
}
static inline void Vorn_WriteHeight(s16 *field, u32 height)
{
    *field = height;
}
#define VORN_CENTER_COORDINATE(out_, coordinate_)                                                  \
    do {                                                                                           \
        (out_) = ((u32)(coordinate_) << 8) + 128;                                                  \
    } while (0)
static inline void Vorn_CellCenter(s32 cell, struct ContactPosition *position)
{
    s32 centeredZ;
    s32 x = Mod(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    s32 z = Div(cell, SceneActorChase_MapWidth(SceneActorChase_CurrentMap()));
    position->x = SceneActorChase_CellCenterCoordinate(x);
    Vorn_WriteHeight(&position->y,
                     Vorn_HeightFromAttributes(SceneActorChase_CellProperties(cell)->attributes));
    VORN_CENTER_COORDINATE(centeredZ, z);
    position->z = centeredZ;
}

u8 Vorn_SelectReachableEscapeCell(struct SceneActorRegistryResetView *actor, u32 savedDirection)
{
    struct ContactPosition target;
    struct SceneActorChaseGroup *group;
    s32 tries;
    u32 found = 0;
    s32 range = 2;
    s32 dest;
    do {
        tries = 0;
        do {
            dest = Vorn_SweepEscapeCells(actor, range);
            if (dest != actor->terrainCell) {
                found = 1;
                goto nextRange;
            }
            actor->body.direction += 64;
        } while ((++tries) <= 3);
    nextRange:
        range--;

        actor->body.direction = savedDirection;
        if (found || (range <= 0))
            goto done;
    } while (1);
done:
    Vorn_CellCenter(dest, &target);

    group = &actor->group;
    if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position, &target)) {
        Callback_InvokeActor(actor, group->fallback);
        return 0;
    }
    return 1;
}
