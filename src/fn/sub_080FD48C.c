#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fuzzy: 98%
// Remaining differences: the mask constant is loaded before the flag address.

static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

#define Axe_UpdateRecoverTimer sub_080FD48C
void Axe_UpdateRecoverTimer(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        u32 mask = ~0x100;
        actor->flags2D4 = mask & actor->flags2D4;
        SetPhase(actor, 0, 0, 3);
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FD48C);
#endif
