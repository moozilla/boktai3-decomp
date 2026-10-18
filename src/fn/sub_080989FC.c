#define ActorContact_UpdateColliderPositions sub_080989FC
#define ActorContact_TickAuxiliaryTimers     sub_080982E4
#include "global.h"
#include "scene_actor_behavior.h"
void sub_080982E4(struct SceneActorBehaviorView *);
u8 SceneActorBehavior_IsNearParticipant(struct SceneActorRegistryActor *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
static inline u32 ReadControl(u32 mask, u16 *control)
{
    return *control & mask;
}
u32 sub_080989FC(struct SceneActorBehaviorView *a)
{
    u16 *control;
    struct ContactPosition *position;
    sub_080982E4(a);
    {
        u32 mask = 0x20;
        u32 offset = 0x2D4;
        control = (void *)((u8 *)a + offset);
        if (*control & mask)
            goto done;
    }
    if (ReadControl(0x110, control))
        goto secondary;
    {
        u32 near = (u32)SceneActorBehavior_IsNearParticipant((void *)a) << 24;
        position = (void *)a;
        position = (void *)((u8 *)position + 0x5C);
        if (!near || ReadControl(6, control))
            goto primary;
    }
secondary: {
    struct ContactCollider *collider = &a->secondaryCollider;
    u32 flags = collider->flags;
    u32 disabled = 4;
    disabled &= flags;
    position = (void *)a;
    position = (void *)((u8 *)position + 0x5C);
    if (!disabled) {
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
    }
}
primary:
    Collider_SetPosition(&a->primaryCollider, position, 0);
done:
    return 1;
}
