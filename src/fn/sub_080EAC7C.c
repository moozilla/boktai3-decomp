#include "global.h"
#include "scene_actor_reaction.h"

// confidence=low: stagger reaction; once it finishes, hands control to
// SceneActor_EndReactionAnimation (kind 0x23) and restarts behaviour 3.

void SceneActor_EndReactionAnimation(void);

static inline void SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetState(struct ChaseActor *actor, u32 s, u32 p, u32 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Octopus_UpdateStagger(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->base.restart = 0;
        {
            u16 *d = &actor->base.colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
        {
            u32 bit = 0x20;
            actor->base.motionFlags = *(&bit) | actor->base.motionFlags;
        }
    }
    {
        u32 bit = 0x10;
        actor->base.reactionFlags = *(&bit) | actor->base.reactionFlags;
    }
    if (frame <= 17 && sequence->index == 0) {
        u16 elapsed = sequence->elapsed;
        u8 duration = sequence->duration;
        if (elapsed == (duration != 0) && duration != 1)
            sequence->elapsed = 0;
    }
    if (actor->base.restart) {
        actor->base.stepDone = 0;
        {
            u16 bit = 0x20;
            actor->base.motionFlags &= ~bit;
        }
        {
            u32 *field = &actor->base.behaviorFlags;
            u32 mask = ~0x80;
            *field &= mask;
        }
        SetCtl(actor, SceneActor_EndReactionAnimation, 0x23);
        SetState(actor, 1, 0, 3);
        actor->base.body.direction = actor->base.tracking.heading;
    }
}
