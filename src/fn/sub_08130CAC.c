// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_control_motion.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
// confidence=low: chooses idle or turn behavior after advancing a route descriptor.
#define SceneActorRoute_UpdateAdvancePhase sub_08130CAC
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}

static inline void State(struct SceneActorRegistryResetView *a, u32 state, u32 phase, u32 behavior,
                         u32 zero)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = zero;
    a->field2B1 = 1;
}
static inline void TurnState(struct SceneActorRegistryResetView *a, u32 behavior)
{
    u8 *field = &a->state2A8;
    u32 zero = 0;
    *field = zero;
    a->phase2A9 = zero;
    a->behavior2AA = behavior;
    {
        u32 *counter = &a->counter2AC;
        *counter = zero;
    }
    a->field2B1 = 1;
}
static inline u32 Flags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
void SceneActorRoute_UpdateAdvancePhase(struct SceneActorRegistryResetView *actor)
{
    register u8 *group asm("r5") = (u8 *)&actor->group.common;
    s32 speed, event;
    u16 *scale;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        BeginControlZero(actor, actor->entryTarget228, 1);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
    if (Flags(actor, 0xA02F)) {
        u16 *p = &actor->field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *p = &actor->field2C2;
        u16 value = *p;
        scale = p;
        if (!value)
            *p = 64;
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    group += 0x1FC;
    event = NavMesh_ConsumeProbeEvent(*(void **)group);
    if (!event) {
        void *descriptor = *(void **)group;
        if (SceneActorBehavior_AccelerateTowardDescriptor(actor, descriptor, 32, 32, speed)) {
            State(actor, event, event, event, event);
            return;
        }
    } else {
        void *descriptor = *(void **)group;
        if (SceneActorBehavior_RequestDescriptorTurn(actor, descriptor, 32, 32, speed))
            TurnState(actor, 6);
    }
    actor->counter2AC++;
}
