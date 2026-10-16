#include "global.h"
#ifdef NONMATCHING
// fuzzy: 48.8%
// Open: the original keeps a second copy of the route entry pointer (r6);
// see docs/MGS_GCL_MATCHING.md.
struct RouteHeader {
    u8 pointCount;
    u8 pad1;
    u16 pointsOffset;
};
struct RouteState {
    u8 route, mode, point, field3;
    struct RouteHeader *entry, *header;
    u8 *pointData;
};
struct Map {
    u8 pad0[0x10];
    u16 *routes;
};
extern struct Map *gUnk_030052F4;
u32 Route_Start(struct RouteState *state, u32 route, u32 mode, u32 point)
{
    u16 *routes = gUnk_030052F4->routes;
    struct RouteHeader *entry, *header;
    u8 *points;
    if (route >= routes[0])
        return 0;
    entry = (struct RouteHeader *)((u8 *)routes + (route * 4 + 4));
    header = entry;
    if (entry->pointCount == 0)
        return 0;
    if (point >= entry->pointCount)
        point = 0;
    points = (u8 *)routes + header->pointsOffset;
    state->route = route;
    state->mode = mode;
    state->point = point;
    state->field3 = 0;
    state->header = header;
    state->pointData = points + state->point * 8;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Route_Start);
#endif
