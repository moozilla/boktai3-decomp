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

void Ghost_ChooseRouteDestination(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetControl(actor, actor->savedTarget, 0);
        state[0x5B6] = 0;
    }
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
        SetState(actor, 0, 0, 2);
        {
            struct ContactPosition *position = &actor->body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        *((u8 *)actor + 0x3CD) = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(state + 0x5FC);
    }
    actor->counter2AC++;
}
