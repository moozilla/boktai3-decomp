#include "global.h"
#include "scene_actor_control_phase.h"

#define SceneActorControl_UpdateDirectionalIdle Ivy_UpdateActiveState
struct SceneActorTargetWaitState {
    u8 unknown0[0x634];
    s16 targetMissFrames;
};
void Ivy_UpdateActiveState(void);
void SceneActorReaction_PlayHiddenCue(void);
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
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
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind,
                              u8 zero8, u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero8;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void ResetPhase(struct SceneActorRegistryResetView *actor, u32 behavior, u8 zero8,
                              u32 zero)
{
    actor->state2A8 = zero8;
    actor->phase2A9 = zero8;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}
void SceneActorControl_UpdateTargetApproachPhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorTargetWaitState *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = SceneActorReaction_PlayHiddenCue;
        u32 kind = 0x2A;
        BeginControl(actor, callback, kind);
        state->targetMissFrames = 0;
    }
    if (actor->field2B0) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 value = ~4;
        u32 zero;
        u8 zero8;
        value &= collider->flags;
        zero8 = 0;
        asm("" : "+r"(zero8));
        zero = 0;
        value |= 8;
        collider->flags = value;
        {
            u16 *flags = &actor->flags2C0;
            u32 mask = ~0x800;
            *flags = mask & *flags;
        }
        SetControl(actor, Ivy_UpdateActiveState, 0x22, zero8, zero);
        ResetPhase(actor, 2, zero8, zero);
    }
    actor->counter2AC++;
}
