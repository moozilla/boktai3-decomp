// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reset.h"

// confidence=low: resets reaction state when damage flag 0x4000 is set.
#define SceneActorReaction_ResetFlaggedState sub_08135CC4

void SceneActorMovement_ProjectProbe(void *, u32, u32);
void SceneActorTerrain_ClearCollisionTimers(void *);
void ActorAttachment_ReleasePartsResource(void *, u32, u32);

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    u32 b = behavior;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

u32 SceneActorReaction_ResetFlaggedState(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2DC & 0x4000) {
        u16 *motionFlags;
        u32 bit;
        s32 initial = actor->fieldA8;
        actor->fieldAA = initial;
        SceneActorMovement_ProjectProbe(actor, 0, 0);
        SceneActorTerrain_ClearCollisionTimers(actor);
        {
            u16 *flags = &actor->flags2D6;
            u32 mask = ~0x9CB;
            *flags = mask & *flags;
        }
        bit = 2;
        motionFlags = &actor->flags2D4;
        {
            u32 flags = *motionFlags;
            u32 combined;
            combined = bit;
            asm("" : "+r"(combined));
            combined |= flags;
            *motionFlags = combined;
        }
        SetState(actor, 3, 0, 0);
        actor->colliderA.responseValue = 38;
        if (*motionFlags & bit)
            ActorAttachment_ReleasePartsResource(actor->group.attachment, 3, actor->body.id);
        return 1;
    }
}
