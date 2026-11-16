#include "global.h"
#include "ivy.h"
#include "scene_actor_control_phase.h"
/* The collider's flags as a 16-bit bitfield: the read-modify-write leaves the zeros that the
 * SetControl and ResetPhase stores reuse. */
struct IvyColliderBits {
    u8 unknown0[6];
    u16 flags : 16;
};

#define SceneActorControl_UpdateDirectionalIdle Ivy_UpdateActiveState
void Ivy_UpdateActiveState(void);
void SceneActorReaction_PlayHiddenCue(void);
static inline void BeginControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void ResetPhase(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void SceneActorControl_UpdateTargetApproachPhase(struct ChaseActor *actor)
{
    struct Ivy *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *callback = SceneActorReaction_PlayHiddenCue;
        u32 kind = 0x2A;
        BeginControl(actor, callback, kind);
        state->targetMissFrames = 0;
    }
    if (actor->base.field2B0) {
        struct IvyColliderBits *collider = (void *)&actor->base.colliderA;
        u32 mask = ~4;
        collider->flags &= mask;
        collider->flags |= 8;
        {
            u16 *flags = &actor->base.flags2C0;
            u32 mask = ~0x800;
            *flags = mask & *flags;
        }
        BeginControl(actor, Ivy_UpdateActiveState, 0x22);
        ResetPhase(actor, 2);
    }
    actor->base.counter2AC++;
}
