#include "global.h"
// confidence=low: accelerates along the route and chooses side-step or turn transitions.

#include "scene_actor_control_phase.h"
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorPhase_SelectLinkedAnimation(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
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
static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                              u32 behavior)
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void SetMotion(struct SceneActorRegistryResetView *actor, void *cb, u32 kind)
{
    actor->base.word104 = (u32)cb;
    actor->base.word108 = 0;
    actor->base.word10C = 0;
    actor->base.word110 = kind;
    actor->base.flags114 = 1;
}
void SceneActorReaction_BeginSideStep(void);
void Vorn_UpdateRouteApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 speed;
    u32 event;
    u16 *scale;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, FIELD(actor, 0x260, void *));
    {
        u32 mask = 0xa02f;
        if (actor->base.flags2D4 & mask) {
            u16 *p = &actor->base.field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *p = &actor->base.field2C2;
            u16 v = *p;
            scale = p;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(FIELD(group, 0x1fc, void *));
    if (event == 0) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(group, 0x1fc, void *), 32,
                                                          32, speed)) {
            SetState(actor, 18);
            SetControl(actor, actor->base.savedTarget, 0);
            SetMotion(actor, SceneActorReaction_BeginSideStep, 38);
            actor->base.field2C2 = 64;
            *(u16 *)(group + 0x54) = 0;
            return;
        }
    } else if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(group, 0x1fc, void *), 32, 32,
                                                        speed)) {
        SetState(actor, 20);
        SetControl(actor, actor->base.savedTarget, 0);
    }
    actor->base.counter2AC++;
}
