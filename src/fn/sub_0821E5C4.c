// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
// see docs/MGS_GCL_MATCHING.md.
struct RouteHeader {
    u8 pointCount;
    u8 pad1;
    u16 pointsOffset;
};
struct RouteState {
    u8 route;
    u8 mode;
    u8 point;
    u8 field3;
    struct RouteHeader *header;
    u8 *pointData;
};
struct Map {
    u8 pad0[0x10];
    u16 *routes;
};
u32 Route_Start(struct RouteState *state, u32 route, u32 mode, u32 point)
{
    u16 *routes = gTerrainMap->routes;
    struct RouteHeader *entry;
    register struct RouteHeader *header asm("r6");
    register u8 *points asm("r1");
    if (route >= routes[0])
        return 0;
    entry = (struct RouteHeader *)((u8 *)routes + (route * 4 + 4));
    header = entry;
    if (entry->pointCount == 0)
        return 0;
    if (point >= entry->pointCount)
        point = 0;
    {
        register u32 off asm("r0") = header->pointsOffset;
        points = (u8 *)routes + off;
    }
    state->route = route;
    state->mode = mode;
    state->point = point;
    state->field3 = 0;
    state->header = header;
    {
        register u8 *res asm("r0") = points + state->point * 8;
        state->pointData = res;
    }
    return 1;
}
