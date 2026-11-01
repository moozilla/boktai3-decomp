#include "global.h"
#include "scene_actor_control_phase.h"
/* The collider's flags as a 16-bit bitfield: the read-modify-write leaves the zeros that the
 * SetControl and ResetPhase stores reuse. */
struct IvyColliderBits {
    u8 unknown0[6];
    u16 flags : 16;
};

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
static inline void ResetPhase(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
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
        struct IvyColliderBits *collider = (void *)&actor->colliderA;
        u32 mask = ~4;
        collider->flags &= mask;
        collider->flags |= 8;
        {
            u16 *flags = &actor->flags2C0;
            u32 mask = ~0x800;
            *flags = mask & *flags;
        }
        BeginControl(actor, Ivy_UpdateActiveState, 0x22);
        ResetPhase(actor, 2);
    }
    actor->counter2AC++;
}
