#include "global.h"
#include "scene_actor_reaction.h"
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void Golem_UpdateRouteMotion(void);
void Golem_UpdateChargeBuildup(void);
static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void Golem_UpdateChargeSettle(struct SceneActorRegistryResetView *actor)
{
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateChargeBuildup;
        SetControl(actor, 0x22, target);
        actor->field2B0 = 0;
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->field2B0) {
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->state2A8 = 0;
            actor->phase2A9 = phase;
            actor->behavior2AA = 1;
        }
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    } else
        actor->counter2AC++;
}
