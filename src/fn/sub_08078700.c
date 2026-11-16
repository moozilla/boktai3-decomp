#include "scene_actor_behavior.h"
u8 SceneActorBehavior_IsNearParticipant(struct ChaseActor *);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 Nonzero(u32 value)
{
    if (value)
        return 1;
    return 0;
}
static inline u32 ReadControl(u32 inputMask, u16 *input)
{
    u32 mask = inputMask;
    u16 *field = input;
    u32 value;
    value = *field;
    return value & mask;
}
void SceneActorBehavior_UpdateColliders(struct ChaseActor *base)
{
    struct ContactCollider *secondary;
    struct ContactCollider *primary;
    u16 *control;
    u16 *field;
    struct ContactPosition *position;
    struct RoomActor *self;
    struct RoomActor *actor = (void *)base;
    secondary = &actor->colliderB;
    primary = &actor->colliderA;
    {
        u32 mask = 0x800;
        if (Test(actor->flags2C0, mask)) {
            primary->flags |= 4;
            self = actor;
            field = &self->flags2D6;
            goto clearAndReturn;
        }
    }
    {
        u32 category = actor->category;
        if ((u32)(category - 12) <= 1 || category == 23) {
            u16 *address = &actor->flags2D6;
            u32 mask = ~0x4000;
            mask &= *address;
            *address = mask;
            return;
        }
    }
    if ((u8)(actor->field2B5 - 21) <= 1)
        goto disable;
    {
        u32 mask = 0x8000;
        u16 *address;
        u32 flags;
        address = &actor->flags2D4;
        flags = *address & mask;
        control = address;
        if (!flags)
            goto activeTest;
    }
disable:
    primary->flags |= 4;
    field = &actor->flags2D6;
clearAndReturn: {
    u32 mask = ~0x4000;
    mask &= *field;
    *field = mask;
}
    return;
activeTest: {
    u32 mask = 0x4000;
    u16 *address;
    u32 flags;
    address = &actor->flags2D6;
    flags = *address & mask;
    field = address;
    if (!Nonzero(flags)) {
        primary->flags |= 4;
        return;
    }
}
    {
        u32 mask = ~4;
        mask &= primary->flags;
        primary->flags = mask;
    }
    {
        u32 mask = ~0x4000;
        mask &= *field;
        *field = mask;
    }
    if (ReadControl(0x110, control))
        goto secondaryUpdate;
    {
        u32 near = (u32)SceneActorBehavior_IsNearParticipant((void *)actor) << 24;
        position = &actor->body.position;
        if (!near || ReadControl(6, control))
            goto primaryUpdate;
    }
secondaryUpdate: {
    u32 flags = secondary->flags;
    u32 disabled = 4;
    disabled &= flags;
    self = actor;
    position = &self->body.position;
    if (!disabled) {
        Collider_SetPosition(secondary, position, 0);
        Collider_Refresh(secondary);
    }
}
primaryUpdate:
    Collider_SetPosition(primary, position, 0);
}
