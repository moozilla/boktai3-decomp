// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
#define SceneActorBehavior_UpdateColliders sub_08078700
u8 sub_08078078(struct SceneActorRegistryActor *);
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
    register u32 mask asm("r1") = inputMask;
    register u16 *field asm("r2") = input;
    u32 value;
    value = *field;
    return value & mask;
}
void SceneActorBehavior_UpdateColliders(struct SceneActorBehaviorView *inputActor)
{
    struct SceneActorBehaviorView *actor = inputActor;
    struct ContactCollider *secondary = &actor->secondaryCollider;
    struct ContactCollider *primary = &actor->primaryCollider;
    register u16 *control asm("r8");
    u16 *field;
    struct ContactPosition *position;
    {
        u32 mask = 0x800;
        u32 offset = 0x2C0;
        if (Test(*(u16 *)((u32)actor + offset), mask)) {
            primary->flags |= 4;
            {
                register u32 offset asm("r0") = 0x2D6;
                field = (void *)((u32)actor + offset);
            }
            goto clearAndReturn;
        }
    }
    {
        u32 category = actor->category;
        if ((u32)(category - 12) <= 1 || category == 23) {
            register u16 *address asm("r0") = &actor->flags2D6;
            u32 mask = ~0x4000;
            mask &= *address;
            *address = mask;
            return;
        }
    }
    if ((u8)(actor->kind - 21) <= 1)
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
    {
        register u32 offset asm("r1") = 0x2D6;
        field = (void *)((u32)actor + offset);
    }
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
        u32 near = (u32)sub_08078078((void *)actor) << 24;
        position = (void *)actor;
        position = (void *)((u32)position + 0x5C);
        if (!near || ReadControl(6, control))
            goto primaryUpdate;
    }
secondaryUpdate: {
    register u32 flags asm("r1") = secondary->flags;
    u32 disabled;
    disabled = flags & 4;
    position = (void *)actor;
    asm("" : "+r"(position));
    position = (void *)((u32)position + 0x5C);
    if (!disabled) {
        Collider_SetPosition(secondary, position, 0);
        Collider_Refresh(secondary);
    }
}
primaryUpdate:
    Collider_SetPosition(primary, position, 0);
}
