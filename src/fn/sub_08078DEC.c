#include "scene_actor_behavior.h"
static inline u8 Unavailable(struct SceneActorBehaviorView *actor)
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
void SceneActorBehavior_ActivatePendingControl(struct SceneActorBehaviorView *actor)
{
    u32 *flags;
    u32 clear;
    u32 zero;
    const void *control;
    u32 kind;
    if (Unavailable(actor)) {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
        return;
    }
    {
        u32 mask = 1;
        u32 *field;
        u32 value;
        u32 active;
        field = &actor->flags114;
        value = *field & mask;
        flags = field;
        value = Test(value, value);
        active = value;
        if (!active) {
            actor->savedControl104 = (void *)active;
            actor->allocation108 = (void *)active;
            return;
        }
    }
    clear = ~1;
    *flags &= clear;
    {
        const void **field = &actor->savedControl104;
        control = *field;
    }
    kind = actor->savedKind110;
    {
        u8 *field = &actor->prepared;
        zero = 0;
        *field = 1;
    }
    actor->substate = zero;
    actor->kind = kind;
    actor->control = control;
    actor->field2C8 = zero;
    *flags &= clear;
    {
        unsigned long long offset = 0x82;
        offset *= 2;
        *(const void **)((u32)actor + offset) = (void *)zero;
    }
    {
        void **allocation = &actor->allocation108;
        void *value = *allocation;
        if (value) {
            Callback_InvokeActor(actor->allocationArgument10C, value);
            *allocation = (void *)zero;
        }
    }
}
