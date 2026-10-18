#define RouteCursor_SelectRandomPoint Lun_SelectRandomRouteEntry
#define RouteActor_SetDestination Lun_BeginMoveToPosition
#include "global.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"
void Lun_SelectRandomRouteEntry(struct SceneActorRouteCursor *);
void Lun_BeginMoveToPosition(u8 *, s16 *);
void SceneActorControl_ResetTargetFlags(u8 *);
struct RouteActorDestinationState {
    u8 unknown0[0x42C];
    struct SceneActorRouteCursor route;
    u8 unknown438[12];
    u16 delay;
};
static inline u16 TestFlag(struct SceneActorRegistryActor *p, u32 mask)
{
    mask &= p->flags2D4;
    return mask;
}
void RouteActor_UpdateRandomDestination(struct SceneActorRegistryActor *actor)
{
    struct RouteActorDestinationState *state = actor->context;
    if (!TestFlag(actor, 0x100)) {
        state->delay = 0;
    } else if (state->delay) {
        state->delay--;
    } else {
        struct ContactPosition target;
        struct ContactPosition *p;
        struct SceneActorRouteCursor *route = &state->route;
        s32 cell;
        u16 attr;
        u32 kind;
        s32 height;
        Lun_SelectRandomRouteEntry(route);
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = SceneActorTerrain_GetCellIndex(p);
        attr = *SceneActorTerrain_GetCell(cell);
        kind = attr >> 4;
        {
            u32 mask = 15;
            mask &= attr;
            height = mask << 8;
        }
        switch (kind) {
            case 1:
                height -= ((u8 *)p)[4];
                break;
            case 2:
                height -= ((u8 *)p)[0];
                break;
        }
        p->y = height;
        Lun_BeginMoveToPosition((u8 *)actor, (s16 *)&target);
        SceneActorControl_ResetTargetFlags((u8 *)actor);
    }
}
