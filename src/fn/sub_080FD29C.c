// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

// foundry draft (worker best 92.0%); what differs is mostly register allocation and instruction order
struct ActorGroupPhaseState {
    u8 unknown0[0xe4c];
    u32 flags;
};
static inline void ResetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                              u32 behavior, u32 counter)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}
void ActorGroup_ResetActivePhase(struct SceneActorRegistryResetView *actor)
{
    struct ActorGroupPhaseState *group = actor->context;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (actor->field2B0) {
        u32 z3, z4;
        {
            u32 m;
            register u32 v asm("r1");
            asm("" ::"m"(actor->flags2D4));
            m = ~0x100;
            asm("" : "+r"(m));
            v = m & actor->flags2D4;
            z3 = 0;
            z4 = 0;
            asm("" : "+r"(z3), "+r"(z4));
            actor->flags2D4 = v;
        }
        {
            u32 *g = &group->flags;
            u32 m = ~0x400;
            *g &= m;
        }
        ResetPhase(actor, z3, z3, 3, z4);
    } else {
        actor->counter2AC++;
    }
}
