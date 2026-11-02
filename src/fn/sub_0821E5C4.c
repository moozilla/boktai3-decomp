#include "engine.h"
#include "scene_actor_route.h"

struct RouteBank {
    u16 count, reserved;
    struct SceneActorRouteDefinition definitions[1];
};

u32 Route_Start(struct SceneActorRouteCursor *state, u32 route, u32 mode, u32 point)
{
    struct RouteBank *bank = (void *)gTerrainMap->routes;
    struct SceneActorRouteDefinition *definition;
    union {
        struct SceneActorRouteDefinition *definition;
        u32 pointCount;
        struct SceneActorRoutePoint *points;
    } record;

    if (route >= bank->count)
        return 0;
    record.definition = &bank->definitions[route];
    definition = record.definition;
    if (!definition->pointCount)
        return 0;
    record.pointCount = record.definition->pointCount;
    if (point >= record.pointCount)
        point = 0;

    record.points = (void *)((u8 *)bank + definition->pointOffset);
    state->unknown0[0] = route;
    state->unknown0[1] = mode;
    state->pointIndex = point;
    state->unknown3 = 0;
    state->definition = definition;
    state->point = &record.points[state->pointIndex];
    return 1;
}
