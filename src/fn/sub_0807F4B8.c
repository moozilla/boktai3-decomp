#include "scene_actor_behavior.h"
#include "scene_actor_reset.h"
void SceneActorAttachment_ReplaceResponseSlot(struct RoomActor *, u32, u32, u32);
extern const u8 gUnk_0824E114[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorControl_OnIdle(u8 *);
#include "scene_actor_contact_list.h"
#include "scene_actor_chase_shared.h"
struct SceneActorBehaviorPlayerSlot {
    u8 unknown0[8];
    u8 slot;
};
static inline u16 *CounterAddress(u32 inputOffset)
{
    u8 *base = (void *)gGameState;
    u32 offset = inputOffset;
    return (void *)(base + offset);
}
static inline void IncrementFirst(u16 *input)
{
    u16 *counter = input;
    u32 value = *counter;
    if (*(s16 *)counter <= 9998) {
        *counter = value + 1;
    } else
        *counter = 9999;
}
static inline void SetControl(struct ChaseActor *a, void *p, u32 kind, u32 zero, u8 byteZero)
{
    a->base.field2B2 = 1;
    a->base.stepDone = byteZero;
    a->base.controlKind = kind;
    a->base.target = p;
    a->base.field2C8 = zero;
    SceneActorChase_AndWordFlags(&a->base.pendingControlFlags, ~1);
    a->base.pendingControl = zero;
}
void SceneActorTarget_ResetContactResponse(struct RoomActor *inputActor)
{
    struct ChaseActor *a = (void *)inputActor;
    struct SceneActorRegistryControlGroup *group = &a->group.common;
    struct RoomActor *actor;
    u32 slot;
    u32 zero;
    u8 byteZero;
    {
        struct SceneActorChaseTracking *gate = &a->base.tracking;
        u32 mask = 8;
        zero = SceneActorChase_Test(gate->flags, mask);
        if (zero)
            return;
    }
    {
        u32 mask = 0x1000;
        u32 bits = a->base.motionFlags;
        bits &= mask;
        if (bits) {
            SceneActorContacts_ReleaseNode(a->group.contactNode);
            a->base.motionFlags &= ~mask;
            a->group.contactNode = (void *)zero;
        }
    }
    {
        struct SceneActorRegistryControl *control = &group->control;
        SceneActorChase_AndWordFlags(&a->base.behaviorFlags, 0xFFFEBE3F);
        control->counter = zero;
        byteZero = 0;
        control->field14 = zero;
        control->timer = zero;
    }
    SceneActorReaction_ResetResponseState((u8 *)group + 0x8C, gUnk_0824E114, 0x60D);
    {
        void *control = SceneActorControl_OnIdle;
        u32 kind = 16;
        u32 one;
        u32 clearMask;
        {
            u8 *field = &a->base.field2B2;
            one = 1;
            *field = one;
        }
        a->base.stepDone = byteZero;
        a->base.controlKind = kind;
        a->base.target = control;
        a->base.field2C8 = zero;
        {
            u32 *field = &a->base.pendingControlFlags;
            clearMask = ~1;
            *field &= clearMask;
        }
        a->base.pendingControl = zero;
        {
            u32 state = 3;
            u8 *field = &a->base.state2A8;
            *field = state;
        }
        a->base.phase2A9 = one;
        a->base.behavior2AA = byteZero;
        a->base.counter2AC = zero;
        a->base.field2B1 = one;
        {
            u32 mask = ~0x80;
            a->base.behaviorFlags &= mask;
        }
        a->base.motionFlags &= clearMask;
        {
            u32 mask = ~0x400;
            a->base.motionFlags &= mask;
        }
    }
    *((u8 *)group + 0x206) = 255;
    actor = (void *)a;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
    if (SceneActorChase_Test(gEngineFlags, 0x4800))
        return;
    {
        u32 mask = 0x8000;
        if (actor->statusFlags & mask)
            return;
    }
    {
        struct SceneActorChaseTracking *gate = &actor->tracking;
        slot = ((struct SceneActorBehaviorPlayerSlot *)gate->target)->slot;
    }
    {
        u16 *counter;
        u32 value;
        u32 result;
        if (slot == 0) {
            IncrementFirst(CounterAddress(0x884));
            counter = CounterAddress(0x530);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 1) {
            counter = CounterAddress(0x886);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 2) {
            counter = CounterAddress(0x888);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 3) {
            counter = CounterAddress(0x88A);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        }
        return;
    capped:
        *counter = 9999;
    }
}
