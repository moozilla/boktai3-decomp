#include "global.h"
#include "scene_actor_reaction.h"
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void Golem_UpdateRouteMotion(void);
void Golem_UpdateRecoil(void);
void sub_080C47DC(void *);
static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
void Golem_UpdateSettle(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->base.context;
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateRecoil;
        SetControl(actor, 0x2A, target);
        actor->base.field2B0 = 0;
        {
            u32 bit = 0x2000;
            u32 *flags = &actor->base.flags2CC;
            *flags |= bit;
        }
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.field2B0) {
        *(struct ContactPosition *)(body + 0x69C) = *(struct ContactPosition *)(body + 0x694);
        sub_080C47DC(body);
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
