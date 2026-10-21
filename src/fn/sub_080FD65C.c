#include "global.h"
#include "scene_actor_reset.h"

static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Axe_UpdateRecoverTimer2(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        u32 mask = 0x100;
        actor->flags2D4 = actor->flags2D4 & ~mask;
        SetPhase(actor, 0, 0, 3);
    } else
        actor->counter2AC++;
}
