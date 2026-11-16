#include "global.h"
#include "scene_actor_reaction.h"
u8 Golem_InvokeEnabledBehavior(struct ChaseActor *actor);
void Golem_UpdateRouteMotion(void);
void Golem_UpdateChargeBuildup(void);
static inline u8 TakeFlag2B1(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
void Golem_UpdateChargeSettle(struct ChaseActor *actor)
{
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateChargeBuildup;
        SetControl(actor, 0x22, target);
        actor->base.stepDone = 0;
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.stepDone) {
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = 1;
        }
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
    } else
        actor->base.counter2AC++;
}
