
#ifdef NONMATCHING
// fuzzy: 92.0%
// foundry draft (worker best 92.0%); what differs is mostly register allocation and instruction order
#define ActorGroup_ResetActivePhase sub_080FD29C
#include "global.h"
#include "scene_actor_reset.h"
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
void sub_080FD29C(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupPhaseState *group = actor->context;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        actor->flags2D4 &= ~0x100;
        group->flags &= ~0x400;
        ResetPhase(actor, 0, 0, 3);
    } else {
        actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FD29C);
#endif
