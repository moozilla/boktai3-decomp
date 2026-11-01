#include "scene_actor_control_helpers.h"
static inline void CpuSet_2(u32 control, void *dest, const void *src)
{
    CpuSet(src, dest, control);
}
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *actor,
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
        u8 *field = &actor->state2A8;
        zeroValue = 0;
        *field = zeroValue;
    }
    actor->phase2A9 = zeroValue;
    actor->behavior2AA = one;
    actor->counter2AC = zeroValue;
    actor->field2B1 = one;
    {
        struct ContactPosition *difference = &temporary.delta;
        const struct ContactPosition *position = &actor->body.position;
        difference->x = target->x - position->x;
        difference->y = target->y - position->y;
        difference->z = target->z - position->z;
        *((u8 *)actor + 0x3CD) = Math_Atan2(difference->x, difference->z);
    }
    {
        u32 offset = 0x42C;
        state = (void *)((u8 *)state + offset);
    }
    temporary.fill = zeroValue;
    CpuSet_2(0x05000002, state, &temporary.fill);
}
