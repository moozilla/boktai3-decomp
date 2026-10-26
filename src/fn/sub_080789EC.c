// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorBehaviorView *, u32, u32, u32);
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
    u32 result;
    if (*(s16 *)counter <= 9998)
        result = value + 1;
    else {
        u32 limit = 9999;
        asm("" : : "r"(limit));
        result = limit;
    }
    *counter = result;
}
void SceneActorBehavior_RecordDefeat(struct SceneActorBehaviorView *inputActor)
{
    struct SceneActorBehaviorView *early = inputActor;
    struct SceneActorBehaviorView *actor;
    u32 slot;
    {
        struct SceneActorBehaviorGateView *gate = &inputActor->gate;
        u32 mask = 8;
        if (Test(gate->flags, mask))
            return;
    }
    {
        u32 *field = &early->flags2CC;
        u32 mask = ~0x80;
        *field &= mask;
    }
    {
        u32 offset = 0x2D4;
        u16 *field;
        u32 mask;
        field = (void *)((u32)early + offset);
        mask = ~1;
        mask &= *field;
        *field = mask;
    }
    actor = early;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
    if (Test(gEngineFlags, 0x4800))
        return;
    {
        u32 mask = 0x8000;
        u32 offset = 0x2D0;
        if (*(u32 *)((u32)actor + offset) & mask)
            return;
    }
    slot = ((struct SceneActorBehaviorPlayerSlot *)actor->gate.object)->slot;
    {
        u16 *counter;
        u32 value;
        register u32 result asm("r0");
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
    capped: {
        register u32 limit asm("r1") = 9999;
        asm("" : : "r"(limit));
        result = limit;
    }
        *counter = result;
    }
}
