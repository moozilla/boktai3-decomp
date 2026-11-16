#include "global.h"
#include "scene_actor_control_phase.h"
struct AdjacentWaitContext {
    u8 pad[0x634];
    s16 missed, limit;
};
struct AdjacentWaitTarget {
    u8 pad[0xC];
    struct ContactPosition position;
};
s32 SceneActorReaction_FindAdjacentTarget(struct ChaseActor *, s32);
s32 Math_Atan2(s32, s32);
void sub_080E9C20(void);
static inline void ResetState(struct ChaseActor *a, u32 zero)
{
    a->base.state2A8 = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = zero;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
static inline void BeginControl(struct ChaseActor *actor, s32 zero)
{
    u32 three = 3;
    u32 stateOffset = 0x2A8;
    u32 phaseOffset = 0x2A9;
    u8 *field = (u8 *)actor + stateOffset;
    *field = three;
    field = (u8 *)actor + phaseOffset;
    *field = zero;
    stateOffset += 2;
    field = (u8 *)actor + stateOffset;
    *field = zero;
    phaseOffset += 3;
    *(u32 *)((u8 *)actor + phaseOffset) = zero;
    stateOffset += 7;
    field = (u8 *)actor + stateOffset;
    *field = 1;
    {
        void *callback = sub_080E9C20;
        stateOffset += 1;
        *((u8 *)actor + stateOffset) = 1;
        phaseOffset = 0x2B0;
        *((u8 *)actor + phaseOffset) = zero;
        stateOffset += 3;
        *((u8 *)actor + stateOffset) = three;
        phaseOffset -= 0x94;
        *(void **)((u8 *)actor + phaseOffset) = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->base.word104 = zero;
    }
}
void SceneActorReaction_WaitForAdjacentTarget(struct ChaseActor *a)
{
    struct AdjacentWaitContext *ctx = a->base.context;
    struct AdjacentWaitTarget *target = (void *)a->base.tracking.target;
    s32 found;
    s32 zero;
    if (SceneActorControl_TakeFlag(&a->base.field2B1))
        ctx->missed = 0;
    found = SceneActorReaction_FindAdjacentTarget(a, 1);
    if (!found) {
        if (ctx->missed > ctx->limit)
            ResetState(a, found);
        else
            ctx->missed++;
    } else
        ctx->missed = 0;
    if ((s32)a->base.counter2AC > 30 && (zero = ctx->missed) == 0) {
        struct ContactPosition *position = &a->base.body.position;
        a->base.body.direction =
            Math_Atan2(target->position.x - position->x, target->position.z - position->z);
        BeginControl(a, zero);
    }
    a->base.counter2AC++;
}
