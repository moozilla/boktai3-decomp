#include "global.h"
#include "scene_actor_reset.h"

static inline void SetPhase(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Axe_UpdateRecoverTimer2(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.stepDone) {
        u32 mask = 0x100;
        actor->base.motionFlags = actor->base.motionFlags & ~mask;
        SetPhase(actor, 0, 0, 3);
    } else
        actor->base.counter2AC++;
}
