#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"


struct GhostRouteEntry {
    u8 unknown0[4];
    u8 *data;
    u16 *point;
};
u32 Ghost_SelectRandomRouteEntry(struct GhostRouteEntry *);
void Ghost_SetMovementEndpoints(struct SceneActorRegistryResetView *, struct ContactPosition *);
u8 sub_08097798(void *);
s32 Math_Atan2(s32, s32);
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void RoutePosition(struct ContactPosition *p, struct GhostRouteEntry *route)
{
    p->x = route->point[0];
    p->z = route->point[1];
    p->y = Chase_Height(p);
}
static inline void ClearVector(u32 *zero, struct ContactPosition *v)
{
    *zero = 0;
    CpuSet(zero, v, 0x05000002);
}
void Ghost_ChooseMovementDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct {
        u32 zero;
        struct ContactPosition target;
    } local;
    SceneActorControl_TakeFlag(&actor->field2B1);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((s32)actor->counter2AC > 60) {
        struct GhostRouteEntry *route;
        struct ContactPosition *p, *q, *from;
        route = (struct GhostRouteEntry *)(state + 0x60C);
        Ghost_SelectRandomRouteEntry(route);
        RoutePosition(&local.target, route);
        p = &local.target;
        Ghost_SetMovementEndpoints(actor, p);
        sub_08097798(actor);
        SetState(actor, 0, 0, 1);
        /* An empty statement, as a macro compiled to nothing leaves it. */
        do {
        } while (0);
        from = &actor->body.position;
        q = &local.target;
        q->x = q->x - from->x;
        p->y = p->y - from->y;
        p->z = p->z - from->z;
        *((u8 *)actor + 0x3CD) = Math_Atan2(q->x, p->z);
        ClearVector(&local.zero, (struct ContactPosition *)(state + 0x5FC));
        return;
    }
    actor->counter2AC++;
}
