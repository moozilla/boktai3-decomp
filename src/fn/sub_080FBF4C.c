#include "global.h"
#include "scene_actor_reset.h"

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
        const u16 *table = gRandomTable;
        return *(u16 *)(offset + (u32)table);
    }
}
inline static u32 Random_SelectDistantWaypoint(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        u32 offset = gUnk_03005308 * 2;
        const u16 *table = *((typeof(gUnk_0203B400) *)0x0203B400);
        return *((u16 *)(offset + ((u32)table)));
    }
}

void ActorPhase_SelectDistantWaypoint(struct SceneActorRegistryResetView *actor,
                                      struct ContactPosition *out,
                                      struct RouteSelectionPosition *origin, s32 limit)
{
    struct RoutePointSelection route;
    struct RoutePointSelection point;
    struct RouteSelectionPosition delta;
    s32 bestDistance;
    s32 i;
    s32 distance;
    unsigned long long wide;
    u16 *entry;
    union {
        s32 value;
        u8 bytes[4];
    } bestIndex;
    Route_Start((void *)(&route), *((s16 *)(((u8 *)actor->context) + 0xeb6)), 0, 0);
    bestDistance = 0;
    bestIndex.value = -1;
    for (i = 0; i < (*route.data); i++) {
        point.entry = (u16 *)(Path_GetPointData(route.data) + (i * 8));
        delta = *origin;
        delta.x = point.entry[0] - (*((u16 *)(&delta)));
        {
            u16 z = point.entry[1] - (*(((u16 *)(&delta)) + 2));
            *(((u32 *)(&delta)) + 1) = ((*(((u32 *)(&delta)) + 1)) & 0xffff0000) | z;
        }
        distance = ((*((s16 *)(&delta))) * (*((s16 *)(&delta)))) +
                   ((*(((s16 *)(&delta)) + 2)) * (*(((s16 *)(&delta)) + 2)));
        if (((distance <= limit) && (bestDistance < distance)) &&
            (!Axe_IsRoutePointRecent(actor, i))) {
            bestIndex.value = i;
            bestDistance = distance;
        }
    }

    if (!bestDistance)
        bestIndex.value = Mod(Random_SelectDistantWaypoint(), *route.data);
    Axe_RecordRoutePoint(actor, bestIndex.value);
    {
        struct RoutePointSelection *dst = &route;
        u32 mode = 0;
        dst->index = bestIndex.bytes[0];
        dst->mode = mode;
    }
    route.entry = (u16 *)Path_GetPointData(route.data);
    route.entry = (u16 *)(((u8 *)route.entry) + (route.index * 8));
    wide = route.entry[0];
    out->x = wide;
    out->y = actor->groundHeight2BE;
    entry = route.entry;
    out->z = entry[1];
}
