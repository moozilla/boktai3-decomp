#include "global.h"
#include "scene_actor_control_phase.h"
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
void Vorn_UpdateApproachRange(struct SceneActorRegistryResetView *);
static inline u8 PhaseComplete(struct SceneActorRegistryResetView *p)
{
    return p->field2B0;
}
void SceneActorPhase_BeginTimedMotion(struct SceneActorRegistryResetView *p)
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
    if (PhaseComplete(p)) {
        u32 state = 3;
        u16 one = 1;
        u8 *field = &p->state2A8;
        u32 zero = 0;
        *field = state;
        p->phase2A9 = one;
        p->behavior2AA = zero;
        p->counter2AC = zero;
        p->field2B1 = one;
        {
            void *control = p->savedTarget;
            u32 *flags;
            p->field2B2 = one;
            *(u8 *)((u32)p + 0x2B0) = zero;
            *(u8 *)((u32)p + 0x2B5) = zero;
            p->target = control;
            p->field2C8 = zero;
            flags = &p->flags114;
            {
                s32 mask = -2;
                *flags &= mask;
            }
            {
                u16 duration;
                u32 *word104 = &p->word104;
                u32 handler = (u32)Vorn_UpdateApproachRange;
                duration = 40;
                *word104 = handler;
                p->word108 = zero;
                p->word10C = zero;
                p->word110 = duration;
                *flags = one;
            }
        }
    } else
        ++p->counter2AC;
}
