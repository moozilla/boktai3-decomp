#include "global.h"
#include "scene_actor_reset.h"

// foundry draft (worker best 92.0%); what differs is mostly register allocation and instruction order
struct ActorGroupPhaseState {
    u8 unknown0[0xe4c];
    u32 flags;
};
static inline void ResetPhase(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior,
                              u32 counter)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = counter;
    actor->base.field2B1 = 1;
}
void ActorGroup_ResetActivePhase(struct ChaseActor *actor)
{
    struct ActorGroupPhaseState *group = actor->base.context;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.stepDone) {
        u32 z3, z4;
        {
            u32 m;
            int c = 0x100;
            u16 *f = &actor->base.motionFlags;
            m = ~c;
            actor->base.motionFlags = m & *f;
            z3 = 0;
            z4 = 0;
        }
        {
            u32 *g = &group->flags;
            u32 m = ~0x400;
            *g &= m;
        }
        ResetPhase(actor, z3, z3, 3, z4);
    } else {
        actor->base.counter2AC++;
    }
}
