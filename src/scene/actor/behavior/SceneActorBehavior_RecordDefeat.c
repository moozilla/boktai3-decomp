#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/actor/scene_actor_behavior.h"
void SceneActorAttachment_ReplaceResponseSlot(struct RoomActor *, u32, u32, u32);
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
        if (SceneActorChase_TrackingFlags(gate, 8))
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
    if (SceneActorChase_Test(gEngineFlags, 0x4800))
        return;
    {
        u32 mask = 0x8000;
        if (actor->statusFlags & mask)
            return;
    }
    slot = actor->tracking.target->playerIndex;
    {
        u16 *counter;
        u32 value;
        u32 result;
        if (slot == 0) {
            IncrementFirst(&gGameState->actorDefeatsByPlayer[0]);
            counter = CounterAddress(0x530);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 1) {
            counter = &gGameState->actorDefeatsByPlayer[1];
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 2) {
            counter = &gGameState->actorDefeatsByPlayer[2];
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 3) {
            counter = &gGameState->actorDefeatsByPlayer[3];
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
