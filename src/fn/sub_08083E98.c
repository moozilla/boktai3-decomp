// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *actor,
                                          const struct ContactPosition *target)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct {
        u32 fill;
        struct ContactPosition delta;
    } temporary;
    u32 zeroValue;
    sub_08083BFC(actor, target);
    sub_08083C38(actor);
    {
        register u32 one asm("r1") = 1;
        u32 offset = 0x2A8;
        u32 phaseOffset;
        register u8 *field asm("r0");
        field = (u8 *)actor + offset;
        zeroValue = 0;
        *field = zeroValue;
        phaseOffset = 0x2A9;
        field = (u8 *)actor + phaseOffset;
        *field = zeroValue;
        offset += 2;
        field = (u8 *)actor + offset;
        *field = one;
        phaseOffset += 3;
        field = (u8 *)actor + phaseOffset;
        *(u32 *)field = zeroValue;
        offset += 7;
        field = (u8 *)actor + offset;
        *field = one;
    }
    {
        struct ContactPosition *difference = &temporary.delta;
        const struct ContactPosition *position = &actor->position;
        asm("" : "+r"(difference));
        difference->x = target->x - position->x;
        difference->y = target->y - position->y;
        difference->z = target->z - position->z;
        *((u8 *)actor + 0x3CD) = Math_Atan2(difference->x, difference->z);
    }
    {
        register u32 offset asm("r2") = 0x42C;
        asm("" : "+r"(offset));
        state = (void *)((u8 *)state + offset);
        asm("" : "+r"(state));
    }
    temporary.fill = zeroValue;
    {
        register void *destination asm("r1");
        const void *source = &temporary.fill;
        u32 control;
        asm("" : "+r"(source));
        destination = state;
        control = 0x05000002;
        CpuSet(source, destination, control);
    }
}
