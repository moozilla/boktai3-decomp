#include "global.h"
#include "scene_actor_control_phase.h"
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
void Vorn_UpdateApproachRange(struct ChaseActor *);
static inline u8 PhaseComplete(struct ChaseActor *p)
{
    return p->base.field2B0;
}
void SceneActorPhase_BeginTimedMotion(struct ChaseActor *p)
{
    if (SceneActorControl_TakeFlag(&p->base.field2B1)) {
        void *control = p->base.onEntry;
        u32 kind = 2;
        SelectControl(p, control, kind);
    }
    if (SceneActor_IsFacingNearbyTarget(p)) {
        if ((u8)Callback_InvokeActor(p, p->base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(p))
            return;
    }
    if (PhaseComplete(p)) {
        u32 state = 3;
        u16 one = 1;
        u8 *field = &p->base.state2A8;
        u32 zero = 0;
        *field = state;
        p->base.phase2A9 = one;
        p->base.behavior2AA = zero;
        p->base.counter2AC = zero;
        p->base.field2B1 = one;
        {
            void *control = p->base.onStanding;
            u32 *flags;
            p->base.field2B2 = one;
            *(u8 *)((u32)p + 0x2B0) = zero;
            *(u8 *)((u32)p + 0x2B5) = zero;
            p->base.target = control;
            p->base.field2C8 = zero;
            flags = &p->base.flags114;
            {
                s32 mask = -2;
                *flags &= mask;
            }
            {
                u16 duration;
                u32 *word104 = &p->base.word104;
                u32 handler = (u32)Vorn_UpdateApproachRange;
                duration = 40;
                *word104 = handler;
                p->base.word108 = zero;
                p->base.word10C = zero;
                p->base.word110 = duration;
                *flags = one;
            }
        }
    } else
        ++p->base.counter2AC;
}
