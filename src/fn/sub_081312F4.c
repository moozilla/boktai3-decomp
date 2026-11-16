#include "global.h"
#include "scene_actor_control_phase.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
u8 sub_0812F3CC(struct ChaseActor *);
// confidence=low: selects a route return phase from motion and route flags.
#define SceneActorRoute_UpdateReturnEntry sub_081312F4

static inline void BeginControl(struct ChaseActor *actor, void *control, u32 kind, u32 zero)
{
    u32 offset;
    {
        u8 *field = &actor->base.field2B2;
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
static inline void State(struct ChaseActor *a, u32 behavior, u32 zero)
{
    a->base.state2A8 = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
static inline void *SavedControl(u8 *group)
{
    void **field = (void *)(group + 0x214);
    return *field;
}
static inline u32 MotionFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}
void SceneActorRoute_UpdateReturnEntry(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        BeginControlZero(actor, actor->base.onEntry, 2);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    {
        u8 *finished = &actor->base.stepDone;
        if (*finished) {
            u16 flags = MotionFlags(actor, 0x10);
            if (flags) {
                State(actor, 8, result);
                BeginControl(actor, SavedControl(group), 0x13, result);
            } else {
                u32 bits = actor->base.behaviorFlags & 0x10;
                if (bits)
                    State(actor, 7, flags);
                else
                    State(actor, 2, bits);
                BeginControlZero(actor, actor->base.onAlternateEntry, 1);
            }
        } else
            actor->base.counter2AC++;
    }
}
