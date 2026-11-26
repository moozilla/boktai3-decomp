#include "actors/scene_actor_behavior/scene_actor_behavior.h"
static inline u8 Unavailable(struct RoomActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        u32 value;
        value = *(u16 *)((u32)actor + offset);
        if (value & mask)
            return 1;
        mask = 0x20000;
        offset = 0x2D0;
        value = *(u32 *)((u32)actor + offset);
        if (value & mask || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorBehavior_ActivatePendingControl(struct RoomActor *actor)
{
    u32 *flags;
    u32 clear;
    u32 zero;
    const void *control;
    u32 kind;
    if (Unavailable(actor)) {
        u32 *field = &actor->pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
        return;
    }
    {
        u32 mask = 1;
        u32 *field;
        u32 value;
        u32 active;
        field = &actor->pendingControlFlags;
        value = *field & mask;
        flags = field;
        value = Test(value, value);
        active = value;
        if (!active) {
            actor->pendingControl = (void *)active;
            actor->pendingControlData = (void *)active;
            return;
        }
    }
    clear = ~1;
    *flags &= clear;
    {
        const void **field = &actor->pendingControl;
        control = *field;
    }
    kind = actor->pendingControlKind;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    actor->stepDone = zero;
    actor->controlKind = kind;
    actor->target = control;
    actor->field2C8 = zero;
    *flags &= clear;
    {
        unsigned long long offset = 0x82;
        offset *= 2;
        *(const void **)((u32)actor + offset) = (void *)zero;
    }
    {
        void **allocation = &actor->pendingControlData;
        void *value = *allocation;
        if (value) {
            Callback_InvokeActor(actor->pendingControlRelease, value);
            *allocation = (void *)zero;
        }
    }
}
