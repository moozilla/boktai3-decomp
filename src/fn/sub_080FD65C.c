#include "global.h"
#include "scene_actor_reset.h"

static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Axe_UpdateRecoverTimer2(struct SceneActorRegistryResetView *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.field2B0) {
        u32 mask = 0x100;
        actor->base.flags2D4 = actor->base.flags2D4 & ~mask;
        SetPhase(actor, 0, 0, 3);
    } else
        actor->base.counter2AC++;
}
