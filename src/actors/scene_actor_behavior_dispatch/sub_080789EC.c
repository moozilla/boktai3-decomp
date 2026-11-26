#include "actors/scene_actor_behavior/scene_actor_behavior.h"
void SceneActorAttachment_ReplaceResponseSlot(struct RoomActor *, u32, u32, u32);
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
void SceneActorBehavior_RecordDefeat(struct RoomActor *inputActor)
{
    struct RoomActor *early = inputActor;
    struct RoomActor *actor;
    u32 slot;
    {
        struct SceneActorChaseTracking *gate = &inputActor->tracking;
        u32 mask = 8;
        if (Test(gate->flags, mask))
            return;
    }
    {
        u32 *field = &early->behaviorFlags;
        u32 mask = ~0x80;
        *field &= mask;
    }
    {
        u16 *field;
        u32 mask;
        field = &early->motionFlags;
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
        if (actor->statusFlags & mask)
            return;
    }
    slot = ((struct SceneActorBehaviorPlayerSlot *)actor->tracking.target)->slot;
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
