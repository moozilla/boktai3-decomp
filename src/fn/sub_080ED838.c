#include "global.h"
#include "scene_actor_reaction.h"

void Ivy_UpdateActiveState(void);

static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetStateB(struct ChaseActor *actor)
{
    u32 two = 2;
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    *field = zero;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = two;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
}

void SceneActorReaction_FinishAnimation17(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->base.colliderA.flags;
        u32 flags = *destination;
        u32 mask = 0xFFFB;
        mask &= flags;
        mask |= 8;
        *destination = mask;
        {
            u16 *p = &actor->base.placementFlags;
            u32 mask = 0xFFFFF7FF;
            *p = mask & *p;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    if (actor->base.restart) {
        SetControl(actor, Ivy_UpdateActiveState, 0x22);
        SetStateB(actor);
    }
}
