// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.1%
#include "scene_actor_reset.h"

// fuzzy: 98.1%
// Remaining: route index byte uses r3 instead of r2.
#define ActorRoute_SelectDistantPoint ActorPhase_SelectDistantWaypoint
#define ActorRoute_RememberPoint      Axe_RecordRoutePoint
#define ActorRoute_IsRecentPoint      Axe_IsRoutePointRecent
#define gRandomTableIndex             gUnk_03005308
#define gRandomTable                  gUnk_0203B400
struct RoutePointSelection {
    u16 id;
    u8 index, mode;
    u8 *data;
    u16 *entry;
};
struct RouteSelectionPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
extern u32 gRandomTableIndex;
extern const u16 gRandomTable[];
s32 Mod(s32, s32);
void ActorRoute_RememberPoint(struct SceneActorRegistryResetView *, u32);
u8 ActorRoute_IsRecentPoint(struct SceneActorRegistryResetView *, s32);
u8 *Path_GetPointData(u8 *);
static inline u32 Random(void)
{
    gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
    {
        u32 offset = gRandomTableIndex * 2;
        register const u16 *table asm("r3") = gRandomTable;
        asm("" : "+r"(table));
        return *(u16 *)(offset + (u32)table);
    }
}
void ActorPhase_SelectDistantWaypoint(struct SceneActorRegistryResetView *actor,
                                      struct ContactPosition *out,
                                      struct RouteSelectionPosition *origin, s32 limit)
{
    struct RoutePointSelection route;
    struct RoutePointSelection point;
    struct RouteSelectionPosition delta;
    s32 bestDistance, i, distance;
    unsigned long long wide;
    u16 *entry;
    union {
        s32 value;
        u8 bytes[4];
    } bestIndex;
    Route_Start((void *)&route, *(s16 *)((u8 *)actor->context + 0xeb6), 0, 0);
    bestDistance = 0;
    bestIndex.value = -1;
    for (i = 0; i < *route.data; i++) {
        point.entry = (u16 *)(Path_GetPointData(route.data) + i * 8);
        delta = *origin;
        delta.x = point.entry[0] - *(u16 *)&delta;
        {
            u16 z = point.entry[1] - *((u16 *)&delta + 2);
            *((u32 *)&delta + 1) = (*((u32 *)&delta + 1) & 0xffff0000) | z;
        }
        distance = *(s16 *)&delta * *(s16 *)&delta + *((s16 *)&delta + 2) * *((s16 *)&delta + 2);
        if (distance <= limit && bestDistance < distance && !ActorRoute_IsRecentPoint(actor, i)) {
            bestIndex.value = i;
            bestDistance = distance;
        }
    }
    if (!bestDistance)
        bestIndex.value = Mod(Random(), *route.data);
    ActorRoute_RememberPoint(actor, bestIndex.value);
    {
        register struct RoutePointSelection *dst asm("r0") = &route;
        register u32 mode asm("r1") = 0;
        asm("" : "+r"(dst), "+r"(mode));
        dst->index = bestIndex.bytes[0];
        dst->mode = mode;
    }
    route.entry = (u16 *)Path_GetPointData(route.data);
    route.entry = (u16 *)((u8 *)route.entry + route.index * 8);
    wide = route.entry[0];
    out->x = wide;
    out->y = actor->groundHeight2BE;
    entry = route.entry;
    out->z = entry[1];
}

#else
INCLUDE_ASM("asm/nonmatching", ActorPhase_SelectDistantWaypoint);
#endif
