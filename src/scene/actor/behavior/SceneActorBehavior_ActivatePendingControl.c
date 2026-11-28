#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_behavior.h"
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
    if (SceneActor_ActionsBlocked((void *)actor)) {
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
        control = (const void *)actor->pendingControl;
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
        if (actor->pendingControlData) {
            Callback_InvokeActor((void *)actor->pendingControlRelease,
                                 (void *)actor->pendingControlData);
            actor->pendingControlData = zero;
        }
    }
}
