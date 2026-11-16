#include "global.h"
// confidence=low: follows the route probe, then selects a new route point outside link mode.

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

void sub_080A4798(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
static inline void State(struct ChaseActor *a, u32 behavior)
{
    u32 off = 0x2A8;
    u32 phase;
    u32 zero;
    {
        u8 *q = &FIELD(a, off, u8);
        zero = 0;
        *q = zero;
    }
    phase = 0x2A9;
    FIELD(a, phase, u8) = zero;
    off += 2;
    FIELD(a, off, u8) = behavior;
    phase += 3;
    FIELD(a, phase, u32) = zero;
    {
        u32 b = 0x2B1;
        FIELD(a, b, u8) = 1;
    }
}
static inline void Control(struct ChaseActor *a, void *target)
{
    u32 off = 0x2B2;
    FIELD(a, off, u8) = 1;
    off -= 2;
    FIELD(a, off, u8) = 0;
    off += 5;
    FIELD(a, off, u8) = 1;
    a->base.target = target;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.pendingControlFlags;
        s32 m = ~1;
        *p &= m;
    }
    a->base.pendingControl = 0;
}
// confidence=low: advances Spider toward its route probe and resumes route control on arrival
void Spider_ApproachRouteProbe(struct ChaseActor *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    s32 speed;
    u16 *scale;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    speed = Callback_InvokeActor(actor, FIELD(actor, 0x260, void *));
    {
        u32 mask = 0xA02F;
        if (actor->base.motionFlags & mask) {
            u16 *p = &actor->base.field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *p = &actor->base.field2C2;
            u32 v = *p;
            scale = p;
            if (!v)
                *p = 64;
        }
    }
    if (!actor->base.drawableKind)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
    if (NavMesh_ConsumeProbeEvent(FIELD(nav, 0x1FC, void *))) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(nav, 0x1FC, void *), 32, 32,
                                                     speed)) {
            State(actor, 13);
            return;
        }
    }
    if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 32, 32,
                                                      speed)) {
        State(actor, 12);
        Control(actor, FIELD(nav, 0x48, void *));
        sub_080A4798(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
    } else {
        actor->base.counter2AC++;
        {
            u32 mask = 0x2000;
            u32 *p = &actor->base.behaviorFlags;
            *p |= mask;
        }
    }
}
