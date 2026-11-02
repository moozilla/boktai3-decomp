// MATCHING_CONCESSION: register-pinning
#include "engine.h"
#include "scene_actor_route.h"

struct RouteBank {
    u16 count, reserved;
    struct SceneActorRouteDefinition definitions[1];
};

u32 Route_Start(struct SceneActorRouteCursor *state, u32 route, u32 mode, u32 point)
{
    struct RouteBank *bank = (void *)gTerrainMap->routes;
    struct SceneActorRouteDefinition *entry, *definition;
    u8 *pointData;
    register u32 address asm("r0");

    if (route >= bank->count)
        return 0;
    entry = &bank->definitions[route];
    address = (u32)entry;
    definition = (void *)address;
    if (entry->pointCount == 0)
        return 0;
    if (point >= entry->pointCount)
        point = 0;

    address = definition->pointOffset;
    pointData = (u8 *)bank + address;
    state->unknown0[0] = route;
    state->unknown0[1] = mode;
    state->pointIndex = point;
    state->unknown3 = 0;
    state->definition = definition;
    address = state->pointIndex * sizeof(*state->point);
    address = (u32)pointData + address;
    state->point = (void *)address;
    return 1;
}
