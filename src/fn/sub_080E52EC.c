// confidence=low: approaches the navigation probe and selects the next dog behavior.
#include "global.h"
/* SceneActorControl_UpdateProbeApproach: control phase that moves the actor toward its nav probe descriptor (+0x5D0): turns on a probe event, otherwise accelerates; changes state on arrival */
#include "scene_actor_control_phase.h"
u8 SceneActorReaction_IsRangeReady(struct ChaseActor *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorMotion_UpdateTargetTransition(void);
u8 Dog_IsActionTimerExpired(void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

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
static inline void ResetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}
static inline void SetState(struct ChaseActor *actor)
{
    u32 behavior = 18;
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
// confidence=low: approaches the navigation probe and selects the next dog state.
void Dog_UpdateProbeApproach(struct ChaseActor *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    u8 *control = (u8 *)SceneActorControl_GetState(actor);
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActorMotion_UpdateTargetTransition, 0x2A);
        {
            u32 bit = 8;
            u32 *flags = (u32 *)(control + 0x678);
            *flags |= bit;
        }
    }
    speed = Callback_InvokeActor(*(&actor), *((void **)(((u8 *)actor) + 0x260)));
    actor->base.field2C2 = 0x20;
    {
        u32 mask = 0xA02F;
        if (actor->base.flags2D4 & mask) {
            u16 *p = &actor->base.field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->base.field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0) {
                *scale = 0x40;
            }
        }
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(FIELD(nav, 0x1FC, void *));
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(nav, 0x1FC, void *), 0x20, 0x20,
                                                     speed)) {
            SetState(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 0x20,
                                                             0x20, speed)) {
        ResetState(actor, 3, 1, 0);
        {
            u32 bit = 4;
            u32 *flags = (u32 *)(control + 0x678);
            *flags |= bit;
        }
        return;
    }
    if (Dog_IsActionTimerExpired(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(actor)) {
            return;
        }
    }
    actor->base.counter2AC++;
}
