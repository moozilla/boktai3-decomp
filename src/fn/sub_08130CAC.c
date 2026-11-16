#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_control_motion.h"
u8 sub_0812F3CC(struct ChaseActor *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
// confidence=low: chooses idle or turn behavior after advancing a route descriptor.
#define SceneActorRoute_UpdateAdvancePhase sub_08130CAC
static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->base.field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->base.controlKind = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}

static inline void State(struct ChaseActor *a, u32 state, u32 phase, u32 behavior, u32 zero)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
static inline void TurnState(struct ChaseActor *a, u32 behavior)
{
    u8 *field = &a->base.state2A8;
    u32 zero = 0;
    *field = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = behavior;
    {
        u32 *counter = &a->base.counter2AC;
        *counter = zero;
    }
    a->base.field2B1 = 1;
}
static inline u32 Flags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}
void SceneActorRoute_UpdateAdvancePhase(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed, event;
    void **routeField;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        BeginControlZero(actor, actor->base.onAlternateEntry, 1);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
    if (Flags(actor, 0xA02F)) {
        u16 *p = &actor->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *p = &actor->base.field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *p = 64;
    }
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);

    routeField = &group->route;
    event = NavMesh_ConsumeProbeEvent(*routeField);
    if (!event) {
        void *descriptor = *routeField;
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, descriptor, 32, 32, speed)) {
            State(actor, event, event, event, event);
            return;
        }
    } else {
        void *descriptor = *routeField;
        if (SceneActorBehavior_RequestDescriptorTurn(actor, descriptor, 32, 32, speed))
            TurnState(actor, 6);
    }
    actor->base.counter2AC++;
}
