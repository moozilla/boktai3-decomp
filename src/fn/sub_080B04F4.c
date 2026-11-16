#include "global.h"
/* SceneActorControl_UpdateProbeApproach_080B04F4: SceneActorControl_UpdateProbeApproach (SceneActorControl_UpdateProbeApproach) with control SceneActorPhase_SelectLinkedAnimation and halfword flags at +0x6D4 (provisional name) */

#include "scene_actor_control_phase.h"
u8 SceneActor_IsFacingNearbyTarget(struct ChaseActor *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct ChaseActor *);
void SceneActorPhase_SelectLinkedAnimation(void);
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
    u32 behavior = 22;
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void SceneActorControl_UpdateProbeApproach_080B04F4(struct ChaseActor *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    u8 *control = (u8 *)SceneActorControl_GetState(actor);
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl(actor, SceneActorPhase_SelectLinkedAnimation, 0x27);
        {
            u32 bit = 0x20;
            u16 *flags = (u16 *)(control + 0x6D4);
            bit |= *flags;
            *flags = bit;
        }
    }
    if (SceneActor_IsFacingNearbyTarget(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(actor)) {
            return;
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
            SetControl(actor, actor->base.onStanding, 0);
            return;
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 0x20,
                                                             0x20, speed)) {
        ResetState(actor, 3, 1, 0);
        {
            u32 bit = 0x40;
            u16 *flags = (u16 *)(control + 0x6D4);
            bit |= *flags;
            *flags = bit;
        }
        return;
    }
    actor->base.counter2AC++;
}
