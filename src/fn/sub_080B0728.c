#include "global.h"
#include "scene_actor_control_phase.h"
/* Tentative phase-control interpretation; names have low confidence. */
u8 sub_0812F3CC(struct ChaseActor *);
static inline void SelectControl(struct ChaseActor *p, void *control, u32 kind)
{
    p->base.field2B2 = 1;
    p->base.field2B0 = 0;
    p->base.field2B5 = kind;
    p->base.target = control;
    p->base.field2C8 = 0;
    {
        u32 *flags = &p->base.flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    p->base.word104 = 0;
}
u8 SceneActor_IsFacingNearbyTarget(struct ChaseActor *);
void SceneActorPhase_UpdateConditionalMotion(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.entryTarget224;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (SceneActor_IsFacingNearbyTarget(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (p->base.field2B0) {
        u32 phase = 0x15;
        p->base.state2A8 = 0;
        p->base.phase2A9 = 0;
        p->base.behavior2AA = phase;
        p->base.counter2AC = 0;
        p->base.field2B1 = 1;
        SelectControl(p, p->base.savedTarget, 0);
    } else
        ++p->base.counter2AC;
}
