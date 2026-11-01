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
struct AxeFlags16 {
    u16 bits : 16;
};
static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
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
        struct AxeFlags16 *flags = (void *)&actor->flags2C0;
        u32 mask = ~0x1800;
        flags->bits &= mask;
        BeginControl(actor, actor->savedTarget, 0);
        SetBehavior(actor, 3);
    } else {
        actor->counter2AC++;
    }
}
