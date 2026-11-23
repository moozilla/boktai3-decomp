#include "scene_actor_control_helpers.h"
static inline void CpuSet_2(u32 control, void *dest, const void *src)
{
    CpuSet(src, dest, control);
}
void SceneActorControl_StartPositionCurve(struct ChaseActor *actor,
                                          const struct ContactPosition *target)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct {
        u32 fill;
        struct ContactPosition delta;
    } temporary;
    u32 zeroValue;
    u32 one;
    SceneActorControl_InitPositionCurveEndpoints(actor, target);
    SceneActorControl_AdvancePositionCurve(actor);
    one = 1;
    {
        u8 *field = &actor->base.state2A8;
        zeroValue = 0;
        *field = zeroValue;
    }
    actor->base.phase2A9 = zeroValue;
    actor->base.behavior2AA = one;
    actor->base.counter2AC = zeroValue;
    actor->base.field2B1 = one;
    {
        struct ContactPosition *difference = &temporary.delta;
        const struct ContactPosition *position = &actor->base.body.position;
        difference->x = target->x - position->x;
        difference->y = target->y - position->y;
        difference->z = target->z - position->z;
        actor->base.nextDirection = Math_Atan2(difference->x, difference->z);
    }
    {
        state = (void *)((u8 *)state + 0x42C);
    }
    temporary.fill = zeroValue;
    CpuSet_2(0x05000002, state, &temporary.fill);
}
