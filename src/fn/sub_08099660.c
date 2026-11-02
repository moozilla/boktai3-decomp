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

void Ghost_ChooseMovementDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    SceneActorControl_TakeFlag(&actor->field2B1);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((s32)actor->counter2AC > 60) {
        struct GhostRouteEntry *route;

        route = (struct GhostRouteEntry *)(state + 0x60C);
        Ghost_SelectRandomRouteEntry(route);
        target = &destination;
        target->x = route->point[0];
        target->z = route->point[1];
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = Chase_HeightAt(Chase_CellIndex(cellX, cellZ), target);
        }

        Ghost_SetMovementEndpoints(actor, endpoint);
        sub_08097798(actor);
        SetState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        *((u8 *)actor + 0x3CD) = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(state + 0x5FC);
        return;
    }
    actor->counter2AC++;
}
