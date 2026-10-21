#include "global.h"
#include "scene_actor_reset.h"

#ifdef NONMATCHING
// fuzzy: 98.7%
// foundry draft (worker best 92.0%); what differs is mostly register allocation and instruction order
struct ActorGroupPhaseState {
    u8 unknown0[0xe4c];
    u32 flags;
};
static inline void ResetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                              u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void ActorGroup_ResetActivePhase(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupPhaseState *group = actor->context;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        {
            u32 m = ~0x100;
            actor->flags2D4 &= m;
        }
        {
            u32 *g = &group->flags;
            u32 m = ~0x400;
            *g &= m;
        }
        ResetPhase(actor, 0, 0, 3);
    } else {
        actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ActorGroup_ResetActivePhase);
#endif
