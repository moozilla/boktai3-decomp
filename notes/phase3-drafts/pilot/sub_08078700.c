#include "scene_actor_behavior.h"
u8 SceneActorBehavior_IsNearParticipant(struct SceneActorRegistryActor *);
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
void SceneActorBehavior_UpdateColliders(struct SceneActorBehaviorView *actor)
{
    struct ContactCollider *secondary = &actor->secondaryCollider;
    struct ContactCollider *primary = &actor->primaryCollider;
    u16 *control;
    struct ContactPosition *position;

    if (Test(actor->flags2C0, 0x800)) {
        primary->flags |= 4;
        actor->flags2D6 &= ~0x4000;
        return;
    }
    if ((u32)(actor->category - 12) <= 1 || actor->category == 23) {
        actor->flags2D6 &= ~0x4000;
        return;
    }
    control = &actor->flags2D4;
    if ((u8)(actor->kind - 21) <= 1 || (*control & 0x8000)) {
        primary->flags |= 4;
        actor->flags2D6 &= ~0x4000;
        return;
    }
    if (!Test(actor->flags2D6, 0x4000)) {
        primary->flags |= 4;
        return;
    }
    primary->flags &= ~4;
    actor->flags2D6 &= ~0x4000;
    if (ReadControl(0x110, control)) {
        position = &actor->position;
    } else {
        u8 near = SceneActorBehavior_IsNearParticipant((void *)actor);
        position = &actor->position;
        if (!near || ReadControl(6, control))
            goto primaryUpdate;
    }
    position = &actor->position;
    if (!(secondary->flags & 4)) {
        Collider_SetPosition(secondary, position, 0);
        Collider_Refresh(secondary);
    }
primaryUpdate:
    Collider_SetPosition(primary, position, 0);
}
