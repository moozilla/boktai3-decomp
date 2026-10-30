#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"

// MATCHING_CONCESSION: register-pinning

struct LunRouteEntry {
    u8 unknown0[4];
    u8 *data;
    u16 *point;
};
void Lun_SelectRandomRouteEntry(struct LunRouteEntry *);
void Lun_SetMovementEndpoints(struct SceneActorRegistryResetView *, struct ContactPosition *);
void sub_0809CCF4(struct SceneActorRegistryResetView *);
s32 Math_Atan2(s32, s32);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
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
static inline void RoutePosition(struct ContactPosition *p, struct LunRouteEntry *route)
{
    p->x = route->point[0];
    p->z = route->point[1];
    p->y = Chase_Height(p);
    // The endpoint setter copies all eight bytes; define the scratch padding.
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Direction(struct ContactPosition *out, const struct ContactPosition *to,
                            const struct ContactPosition *from)
{
    Delta(out, from, to);
    return Math_Atan2(out->x, out->z);
}
static inline void ClearVector(u32 *zero, struct ContactPosition *v)
{
    *zero = 0;
    CpuSet(zero, v, 0x05000002);
}
void Lun_ChooseMovementDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct {
        u32 zero;
        struct ContactPosition target;
    } local;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, actor->savedTarget, 0);
        state[0x3E2] = 0;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((s16)actor->field2EA > 239 && (s32)actor->counter2AC > 60) {
        struct LunRouteEntry *route;
        actor->field2EA = 240;
        route = (struct LunRouteEntry *)(state + 0x42C);
        Lun_SelectRandomRouteEntry(route);
        RoutePosition(&local.target, route);
        Lun_SetMovementEndpoints(actor, &local.target);
        sub_0809CCF4(actor);
        SetState(actor, 0, 0, 1);
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
        ClearVector(&local.zero, (struct ContactPosition *)(state + 0x424));
    }
    actor->counter2AC++;
}
