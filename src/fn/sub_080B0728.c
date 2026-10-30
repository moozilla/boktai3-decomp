#include "global.h"
#include "scene_actor_control_phase.h"
/* Tentative phase-control interpretation; names have low confidence. */
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void SelectControl(struct SceneActorRegistryResetView *p, void *control, u32 kind)
{
    p->field2B2 = 1;
    p->field2B0 = 0;
    p->field2B5 = kind;
    p->target = control;
    p->field2C8 = 0;
    {
        u32 *flags = &p->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    p->word104 = 0;
}
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
void SceneActorPhase_UpdateConditionalMotion(struct SceneActorRegistryResetView *p)
{
    if (SceneActorControl_TakeFlag(&p->field2B1)) {
        void *control = p->entryTarget224;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (SceneActor_IsFacingNearbyTarget(p)) {
        if ((u8)Callback_InvokeActor(p, p->contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (p->field2B0) {
        u32 phase = 0x15;
        p->state2A8 = 0;
        p->phase2A9 = 0;
        p->behavior2AA = phase;
        p->counter2AC = 0;
        p->field2B1 = 1;
        SelectControl(p, p->savedTarget, 0);
    } else
        ++p->counter2AC;
}
