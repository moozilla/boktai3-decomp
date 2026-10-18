#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
struct SceneActorTargetWaitState {
    u8 unknown0[0x634];
    s16 targetMissFrames;
};
s32 sub_080F0910(struct SceneActorRegistryResetView *, s32);
void sub_080ED4E0(void);
static inline void ClearFlags(u16 *p, u32 mask)
{
    *p = mask & *p;
}
static inline void SetCollider(struct ContactCollider *collider)
{
    u32 flags = ~4;
    flags &= collider->flags;
    flags |= 8;
    collider->flags = flags;
}
static inline void BeginTargetControl(struct SceneActorRegistryResetView *actor, s32 zero)
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
        void *callback = sub_080ED4E0;
        stateOffset += 1;
        *((u8 *)actor + stateOffset) = 1;
        phaseOffset = 0x2B0;
        *((u8 *)actor + phaseOffset) = zero;
        stateOffset += 3;
        *((u8 *)actor + stateOffset) = three;
        phaseOffset -= 0x94;
        *(void **)((u8 *)actor + phaseOffset) = callback;
        stateOffset += 0x13;
        *(u16 *)((u8 *)actor + stateOffset) = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->word104 = zero;
    }
}
void SceneActorControl_WaitForTarget(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    s32 zero;
    struct SceneActorTargetWaitState *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SetCollider(&actor->colliderA);
        ClearFlags(&actor->flags2C0, ~0x800);
    }
    if (!sub_080F0910(actor, 1))
        state->targetMissFrames++;
    else
        state->targetMissFrames = 0;
    if ((s32)actor->counter2AC > 30 && (zero = state->targetMissFrames) == 0) {
        BeginTargetControl(actor, zero);
    }
    {
        u32 offset = 0x2AC;
        u32 *counter = (void *)((u8 *)actor + offset);
        *counter += 1;
    }
}
