#include "global.h"
/* SceneActorControl_UpdateEntryRecoveryPhase_080E55AC: same code as SceneActorControl_UpdateEntryRecoveryPhase at another address (ported by relatives.py; provisional name) */
#include "scene_actor_control_phase.h"
u8 Dog_IsActionTimerExpired(struct ChaseActor *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorMotion_TestTargetTransition(void);
static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor)
{
    u32 state = 3;
    u32 phase = 1;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void SceneActorControl_UpdateEntryRecoveryPhase_080E55AC(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->base.field2B0) {
        SetState(actor);
        SetControl(actor, (void *)SceneActorMotion_TestTargetTransition, 43);
    } else
        ++actor->base.counter2AC;
}
