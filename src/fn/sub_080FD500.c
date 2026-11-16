#include "global.h"
#include "axe.h"
#include "scene_actor_control_phase.h"
void Axe_UpdateTargetWindup(void);
void ActorPhase_UpdateSpriteCollapse(void);
static inline void Or16(u32 m, u16 *p)
{
    *p = m | *p;
}
static inline void Or32(u32 m, u32 *p)
{
    *p |= m;
}
static inline void Restart(struct ChaseActor *actor)
{
    void *callback = (void *)ActorPhase_UpdateSpriteCollapse;
    u32 kind = 0x23;
    u8 zero8;
    u8 one;
    u32 zero;
    u8 *taken = &actor->base.field2B2;
    zero = 0;
    one = 1;
    *taken = one;
    actor->base.field2B0 = zero;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    zero8 = 0;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = zero;
    {
        u32 phase = 2;
        u32 behavior = 5;
        actor->base.state2A8 = zero8;
        actor->base.phase2A9 = phase;
        actor->base.behavior2AA = behavior;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    }
}
void SceneActorControl_UpdateControl35Phase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *callback = (void *)Axe_UpdateTargetWindup;
        u32 kind = 0x35;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = kind;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = 0;
        Or16(0x100, &actor->base.flags2D4);
        Or32(0x800, &context->flagsE4C);
        context->fieldEBA = context->fieldEC2;
        Or16(0x1000, &actor->base.flags2C0);
    }
    if (actor->base.field2B0) {
        Restart(actor);
    } else {
        ++actor->base.counter2AC;
    }
}
