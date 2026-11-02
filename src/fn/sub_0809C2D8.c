#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_phase.h"

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

void Lun_ChooseMovementDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

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
        target = &destination;
        target->x = route->point[0];
        target->z = route->point[1];
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = Chase_HeightAt(Chase_CellIndex(cellX, cellZ), target);
        }

        Lun_SetMovementEndpoints(actor, endpoint);
        sub_0809CCF4(actor);
        SetState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        *((u8 *)actor + 0x3CD) = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(state + 0x424);
    }
    actor->counter2AC++;
}
