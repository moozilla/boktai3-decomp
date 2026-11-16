#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActor_AdvanceOffsetApproach(void);
void ActorPhase_UpdateSpriteCollapse(void);
struct SceneActorControl2AContext {
    u8 unknown0[0xEBE];
    u16 fieldEBE;
    u8 unknownEC0[6];
    u16 fieldEC6;
};
static inline void Or16(u32 m, u16 *p)
{
    *p = m | *p;
}
static inline void Or32(u32 m, u32 *p)
{
    *p |= m;
}
static inline u16 Mask16(u16 *p, u32 m)
{
    return *p & m;
}
static inline void Restart(struct SceneActorRegistryResetView *actor)
{
    u8 zero8;
    u32 zero;
    actor->base.flags2D4 = Mask16(&actor->base.flags2D4, ~0x100);
    zero8 = 0;
    zero = 0;
    {
        void *callback = (void *)ActorPhase_UpdateSpriteCollapse;
        u32 kind = 0x23;
        u8 one;
        u8 *taken = &actor->base.field2B2;
        one = 1;
        *taken = one;
        actor->base.field2B0 = zero8;
        actor->base.field2B5 = kind;
        actor->base.target = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = zero;
        {
            u32 behavior = 4;
            actor->base.state2A8 = zero8;
            actor->base.phase2A9 = zero8;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = one;
        }
    }
}
void SceneActorControl_UpdateControl2APhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControl2AContext *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *callback = (void *)SceneActor_AdvanceOffsetApproach;
        u32 kind = 0x2A;
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
        context->fieldEBE = context->fieldEC6;
    }
    if (actor->base.field2B0) {
        Restart(actor);
    } else {
        ++actor->base.counter2AC;
    }
}
