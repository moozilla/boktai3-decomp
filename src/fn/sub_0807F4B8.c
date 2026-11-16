#include "scene_actor_behavior.h"
#include "scene_actor_reset.h"
void SceneActorAttachment_ReplaceResponseSlot(struct RoomActor *, u32, u32, u32);
extern const u8 gUnk_0824E114[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorControl_OnIdle(u8 *);
#include "scene_actor_contact_list.h"
struct SceneActorBehaviorPlayerSlot {
    u8 unknown0[8];
    u8 slot;
};
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
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
static inline void ClearBits(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void SetControl(struct ChaseActor *a, void *p, u32 kind, u32 zero,
                              u8 byteZero)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = byteZero;
    a->base.field2B5 = kind;
    a->base.target = p;
    a->base.field2C8 = zero;
    ClearBits(&a->base.flags114, ~1);
    a->base.word104 = zero;
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
        struct SceneActorChaseTracking *gate = &((struct RoomActor *)a)->tracking;
        u32 mask = 8;
        zero = Test(gate->flags, mask);
        if (zero)
            return;
    }
    {
        u32 mask = 0x1000;
        u32 bits = a->base.flags2D4;
        bits &= mask;
        if (bits) {
            SceneActorContacts_ReleaseNode(a->group.contactNode);
            a->base.flags2D4 &= ~mask;
            a->group.contactNode = (void *)zero;
        }
    }
    {
        struct SceneActorRegistryControl *control = &group->control;
        ClearBits(&a->base.flags2CC, 0xFFFEBE3F);
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
        a->base.field2B0 = byteZero;
        a->base.field2B5 = kind;
        a->base.target = control;
        a->base.field2C8 = zero;
        {
            u32 *field = &a->base.flags114;
            clearMask = ~1;
            *field &= clearMask;
        }
        a->base.word104 = zero;
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
            a->base.flags2CC &= mask;
        }
        a->base.flags2D4 &= clearMask;
        {
            u32 mask = ~0x400;
            a->base.flags2D4 &= mask;
        }
    }
    *((u8 *)group + 0x206) = 255;
    actor = (void *)a;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
    if (Test(gEngineFlags, 0x4800))
        return;
    {
        u32 mask = 0x8000;
        u32 offset = 0x2D0;
        if (*(u32 *)((u32)actor + offset) & mask)
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
