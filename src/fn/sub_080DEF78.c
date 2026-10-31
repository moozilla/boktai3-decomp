#include "scene_actor_chase_reaction.h"

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

struct DogRetreatContext {
    u8 unknown0[0x6A6];
    u8 turnClockwise;
    u8 blockedRoutes;
};

static inline s8 TurnClockwise(void *object)
{
    struct DogRetreatContext *context = object;
    s16 offset = (u32) & ((struct DogRetreatContext *)0)->turnClockwise;
    return *((u8 *)context + offset);
}
#define SetTurnClockwise(object_, value_)                                                          \
    do {                                                                                           \
        struct DogRetreatContext *context_ = (object_);                                            \
        s16 offset_ = (u32) & ((struct DogRetreatContext *)0)->turnClockwise;                      \
        *((u8 *)context_ + offset_) = (value_);                                                    \
    } while (0)
static inline void *GroupRoute(void *object)
{
    struct SceneActorChaseGroup *group = object;
    s16 offset = (u32) & ((struct SceneActorChaseGroup *)0)->route;
    return *(void **)((u8 *)group + offset);
}

static inline u32 CellShape(struct EngineMap **terrain, u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = (*terrain)->cells + (cell * 4 + 12);
    return *data >> 4;
}
static inline s32 FacingQuarter(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}
static inline s32 MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
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
static inline void Dog_CaptureRoutePosition(struct ContactPosition **out,
                                            struct SceneActorRegistryResetView *actor)
{
    *out = &actor->body.position;
}

/* Scan candidate cells, preserve the interruption map owner, then start the route. */
static inline u32 SelectRetreat(struct SceneActorRegistryResetView *actor, u32 priorDirection,
                                s32 steps)
{
    struct DogRetreatContext *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition center;
    struct ContactPosition *routePosition;
    s32 tries = 0;
    s32 chosen;

    do {
        {
            u32 cell, height, shape;
            s32 next, i;
            struct ContactPosition *position;
            struct EngineMap **terrain, **completion;
            cell = actor->terrainCell;
            position = &actor->body.position;
            Dog_CaptureRoutePosition(&routePosition, actor);
            terrain = &gTerrainMap;
            height = SceneActorChase_HeightAt(cell, position);
            shape = CellShape(terrain, cell);
            switch (FacingQuarter(actor->body.direction + 32) & 3) {
                case 0:
                    next = cell + 1;
                    i = 0;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            break;
                        next++;
                        i++;
                    } while (i < steps);
                    chosen = next - 1;
                    goto scanDone;
                case 1:
                    next = cell + MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishSouth;
                        next += MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next - MapWidth(completion);
                    goto scanDone;
                case 2:
                    next = cell - 1;
                    i = 0;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            break;
                        next--;
                        i++;
                    } while (i < steps);
                    chosen = next + 1;
                    goto scanDone;
                case 3:
                    next = cell - MapWidth(terrain);
                    i = 0;
                    completion = (struct EngineMap **)0x030052F4;
                    do {
                        if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                            goto finishNorth;
                        next -= MapWidth(terrain);
                        i++;
                    } while (i < steps);
                    chosen = next + MapWidth(completion);
                    goto scanDone;
                finishSouth:
                    chosen = next - MapWidth(terrain);
                    goto scanDone;
                finishNorth:
                    chosen = next + MapWidth(terrain);
                    goto scanDone;
                default:
                    chosen = actor->terrainCell;
                    goto scanDone;
            }

        scanDone:;
        }
        if (chosen != actor->terrainCell)
            goto selected;
        if (TurnClockwise(context))
            actor->body.direction += 64;
        else
            actor->body.direction -= 64;
        tries++;
    } while (tries <= 3);
selected: {
    u32 index = (gRandomIndex + 1) & 1023;
    s32 value;
    gRandomIndex = index;
    value = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
    if (value <= 127)
        SetTurnClockwise(context, 1);
    else
        SetTurnClockwise(context, 0);
}
    actor->body.direction = priorDirection;
    SceneActorChase_CellCenter(chosen, &center);
    {
        if (TryRoute(actor, GroupRoute(group), routePosition, &center)) {
            context->blockedRoutes++;
            return 1;
        }
        Callback_InvokeActor(actor, group->fallback);
    }
    return 0;
}

u32 sub_080DEF78(struct SceneActorRegistryResetView *actor, u32 priorDirection)
{
    return SelectRetreat(actor, priorDirection, 1);
}
