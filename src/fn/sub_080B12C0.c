#include "global.h"
#include "route_sprite_actor.h"
#include "engine.h"
#include "ram.h"

#define Route_GetPoints Path_GetPointData
struct SceneActorRouteSelection {
    u8 unknown0[0x2be];
    u16 groundHeight;
    u8 unknown2C0[0x6b8 - 0x2c0];
    s8 routeId;
};
void Route_Start(struct RouteSpriteRoute *, s32, u32, u32);
u8 *Path_GetPointData(const void *);
s32 Mod(s32, s32);
struct RouteSelectionPosition {
    u32 x : 16, y : 16, z : 16;
};
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
void SceneActor_SelectDistantRoutePoint(struct SceneActorRouteSelection *p,
                                        struct ContactPosition *out, struct ContactPosition *origin,
                                        s32 limit)
{
    struct SceneActorRouteSelection *actor = p;
    struct RouteSpriteRoute route;
    volatile struct RouteSpriteRoute candidate;
    struct RouteSelectionPosition delta;
    s32 best;
    s32 selected;
    s32 i;

    Route_Start(&route, p->routeId, 0, 0);
    best = 0;
    selected = -1;
    i = 0;
    for (; i < route.resource->pointCount; i++) {
        s32 distance;
        candidate.current = (struct RouteSpriteRoutePoint *)Path_GetPointData(route.resource) + i;
        *(struct ContactPosition *)&delta = *origin;
        {
            u16 x = candidate.current->x - ((struct ContactPosition *)&delta)->x;
            ((u32 *)&delta)[0] = (((u32 *)&delta)[0] & 0xffff0000) | x;
        }
        {
            u16 z = candidate.current->z - ((struct ContactPosition *)&delta)->z;
            ((u32 *)&delta)[1] = (((u32 *)&delta)[1] & 0xffff0000) | z;
        }
        distance = ((struct ContactPosition *)&delta)->x * ((struct ContactPosition *)&delta)->x +
                   ((struct ContactPosition *)&delta)->z * ((struct ContactPosition *)&delta)->z;
        if (distance <= limit && best < distance) {
            selected = i;
            best = distance;
        }
    }
    if (!best) {
        selected = Mod(randomValue(), route.resource->pointCount);
    }
    route.index = selected;
    route.ended = 0;
    route.current = (struct RouteSpriteRoutePoint *)Path_GetPointData(route.resource);
    route.current += route.index;
    {
        struct ContactPosition *dest = out;
        dest->x = route.current->x;
        {
            struct SceneActorRouteSelection *a = actor;
            u32 c = 0x2be;
            u16 *q;
            q = (u16 *)((u32)a + c);
            dest->y = *q;
        }
        dest->z = route.current->z;
    }
}
