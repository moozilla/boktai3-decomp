#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"

// MATCHING_CONCESSION: register-pinning

struct GhostRouteEntry {
    u8 unknown0[4];
    u8 *data;
    u16 *point;
};
u32 Ghost_SelectRandomRouteEntry(struct GhostRouteEntry *);
void Ghost_SetMovementEndpoints(struct SceneActorRegistryResetView *, struct ContactPosition *);
u8 sub_08097798(void *);
s32 Math_Atan2(s32, s32);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    u32 zero = 0;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
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
// confidence=low: after the first frame waits 60 ticks, then picks a random route point and starts moving to it.
void Ghost_ChooseRouteDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct {
        u32 zero;
        struct ContactPosition target;
    } local;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, actor->savedTarget, 0);
        state[0x5B6] = 0;
    }
    if ((s32)actor->counter2AC > 60) {
        struct GhostRouteEntry *route;
        route = (struct GhostRouteEntry *)(state + 0x60C);
        Ghost_SelectRandomRouteEntry(route);
        RoutePosition(&local.target, route);
        Ghost_SetMovementEndpoints(actor, &local.target);
        sub_08097798(actor);
        SetState(actor, 0, 0, 2);
        {
            struct ContactPosition *q;
            struct ContactPosition *p = &local.target;
            struct ContactPosition *from = &actor->body.position;
            asm("" ::: "r7");
            q = &local.target;
            q->x = q->x - from->x;
            p->y = p->y - from->y;
            p->z = p->z - from->z;
            *((u8 *)actor + 0x3CD) = Math_Atan2(q->x, p->z);
        }
        ClearVector(&local.zero, (struct ContactPosition *)(state + 0x5FC));
    }
    actor->counter2AC++;
}
