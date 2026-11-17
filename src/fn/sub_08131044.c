#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_control_motion.h"
#include "debug.h"
u8 sub_0812F3CC(struct ChaseActor *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
// confidence=low: selects control behavior after a route descriptor completes.
#define SceneActorRoute_UpdateRouteCompletion sub_08131044

static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 offset;
    {
        u8 *field = &actor->base.field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = 0u;
    actor->base.controlKind = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = 0u;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0u;
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
static inline u32 FlagsSave(struct ChaseActor *a, u32 mask, u16 **out)
{
    u16 *flags = &a->base.motionFlags;
    u32 value = *flags & mask;
    *out = flags;
    return value;
}
static inline u32 Flags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}
static inline void Control(struct ChaseActor *a, void *cb, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.stepDone = 0;
    a->base.controlKind = kind;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *flags = &a->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    a->base.pendingControl = 0;
}
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRoute_UpdateRouteCompletion(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 speed, event;
    u16 *scale;
    u16 *motionFlags;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
    if (FlagsSave(actor, 0xA02F, &motionFlags)) {
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
    {
        void **descriptor = (void **)(group + 0x1FC);
        event = NavMesh_ConsumeProbeEvent(*descriptor);
        if (event) {
            if (SceneActorBehavior_RequestDescriptorTurn(actor, *descriptor, 32, 32, speed)) {
                TurnState(actor, 6);
                Control(actor, actor->base.onStanding, 0);
            }
        } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *descriptor, 32, 32,
                                                                 speed)) {
            struct ContactPosition **destination;
            u32 destinationOffset;
            u32 zero = event;
            *(u16 *)(group + 0x54) = zero;
            destinationOffset = 0x200;
            destination = (void *)(group + destinationOffset);
            if ((*destination)->pad != 0) {
                u8 *control;
                State(actor, 0, 0, 2, zero);
                SceneActorMovement_ResolveRoutePosition(actor);
                control = group + 0x4C;
                {
                    u32 mask = 0x200000;
                    if (actor->base.behaviorFlags & mask) {
                        {
                            u32 *flags = &actor->base.statusFlags;
                            u32 mask = ~0x1000000;
                            *flags &= mask;
                        }
                        *motionFlags &= ~destinationOffset;
                        control[0xA] = 0;
                        *(u32 *)(control + 0x18) = zero;
                        *(u16 *)(control + 8) = zero;
                    }
                }
                {
                    u32 *flags = &actor->base.behaviorFlags;
                    u32 mask = ~0x90;
                    *flags &= mask;
                }
                actor->base.field2C2 = 64;
                Memory_Clear(*destination, 8);
            } else {
                State(actor, 0, 0, 5, 0);
            }
            Control(actor, actor->base.onStanding, 0);
            return;
        }
    }
    actor->base.counter2AC++;
}
