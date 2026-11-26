#include "scene/actor/scene_actor_reset.h"

// confidence=low: resets reaction state when damage flag 0x4000 is set.

void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorTerrain_ClearCollisionTimers(void *);
void ActorAttachment_ReleasePartsResource(void *, u32, u32);

static inline void SetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    u32 b = behavior;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

u32 SceneActorReaction_ResetFlaggedState(struct ChaseActor *actor)
{
    if (actor->base.hitReactionFlags & 0x4000) {
        u16 *motionFlags;
        u32 bit;
        s32 initial = actor->base.attachmentRestart;
        actor->base.attachmentTimer = initial;
        SceneActorMovement_ProjectProbe(actor, 0, 0);
        SceneActorTerrain_ClearCollisionTimers(actor);
        {
            u16 *flags = &actor->base.reactionFlags;
            u32 mask = ~0x9CB;
            *flags = mask & *flags;
        }
        bit = 2;
        motionFlags = &actor->base.motionFlags;
        {
            u16 mask = bit;
            *motionFlags |= mask;
        }
        SetState(actor, 3, 0, 0);
        actor->base.colliderA.responseValue = 38;
        if (*motionFlags & bit)
            ActorAttachment_ReleasePartsResource(actor->group.attachment, 3, actor->base.body.id);
        return 1;
    }
}
