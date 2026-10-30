#include "global.h"
/* SceneActorControl_UpdateEntryRecoveryPhase_080E55AC: same code as SceneActorControl_UpdateEntryRecoveryPhase at another address (ported by relatives.py; provisional name) */
#include "scene_actor_control_phase.h"
u8 Dog_IsActionTimerExpired(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorMotion_TestTargetTransition(void);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor)
{
    u32 state = 3;
    u32 phase = 1;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorControl_UpdateEntryRecoveryPhase_080E55AC(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SetControl(actor, control, 2);
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->field2B0) {
        SetState(actor);
        SetControl(actor, (void *)SceneActorMotion_TestTargetTransition, 43);
    } else
        ++actor->counter2AC;
}
