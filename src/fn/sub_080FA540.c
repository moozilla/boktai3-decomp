// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

void ActorPhase_UpdateSpriteCollapse(void);
static inline u8 TakePending(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    u32 zero;
    actor->field2B2 = 1;
    zero = 0;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void BeginControl2(struct SceneActorRegistryResetView *actor, void *control,
                                 u32 zero8, u32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero8;
    actor->field2B5 = zero8;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
void Axe_UpdateCollapsePhase(struct SceneActorRegistryResetView *actor)
{
    if (TakePending(actor)) {
        BeginControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        {
            u32 flag = 0x1800;
            flag |= actor->flags2C0;
            actor->flags2C0 = flag;
        }
    }
    if (actor->field2B0) {
        u16 *flags = &actor->flags2C0;
        u32 mask = ~0x1800;
        u32 zero;
        u8 zero8;
        u32 masked;
        asm("" : "+r"(mask));
        masked = mask & *flags;
        zero8 = 0;
        asm("" : "+r"(zero8));
        zero = 0;
        *flags = masked;
        BeginControl2(actor, actor->savedTarget, zero8, zero);
        {
            u32 behavior = 3;
            actor->state2A8 = zero8;
            actor->phase2A9 = zero8;
            actor->behavior2AA = behavior;
        }
        zero8 = zero;
        actor->counter2AC = zero8;
        actor->field2B1 = 1;
    } else {
        actor->counter2AC++;
    }
}
