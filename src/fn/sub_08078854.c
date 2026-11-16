#include "scene_actor_behavior.h"
#include "scene_actor_placement.h"
void Collision_CalculateDamageResponse(struct SceneActorBehaviorContactView *,
                                       struct SceneActorBehaviorContactView *);
void SceneActor_SetContactReactionDirection(struct RoomActor *,
                                            struct SceneActorBehaviorContactView *,
                                            struct SceneActorBehaviorContactView *);
void PlayerStats_AddContactHundredths(struct SceneActorBehaviorContactView *, void *, u32);
static inline u32 Unavailable(struct RoomActor *actor)
{
    return SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor);
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 ReadReactionFlags(u32 inputMask, struct SceneActorBehaviorContactView *source)
{
    u32 mask = inputMask;
    return source->reactionFlags & mask;
}
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *source,
                                             struct SceneActorBehaviorContactView *destination,
                                             struct RoomActor *actor)
{
    struct SceneActorBehaviorContactView *primary;
    u32 unavailable = Unavailable(actor);
    if (unavailable)
        return;
    primary = (void *)&actor->colliderA;
    if (actor->colliderA.responseValue) {
        if (actor->field2E6) {
            destination->remaining = unavailable;
            destination->mode = 0;
        }
        return;
    }
    {
        u32 mask = 4;
        if (primary->flags & mask)
            return;
    }
    {
        u32 mask = 1;
        if (!(source->flags & mask))
            return;
    }
    Collision_CalculateDamageResponse(source, destination);
    destination->parameter44 = source->parameter44;
    {
        u32 mode = destination->mode;
        u32 offset;
        offset = 0x2E6;
        *(u16 *)((u32)actor + offset) = mode;
    }
    if ((u32)(actor->category - 18) <= 2 || actor->category == 23) {
        destination->power = source->power;
    } else if (ReadReactionFlags(0x40000, source)) {
        u8 *defense = &actor->field9F;
        s32 power;
        power = source->power;
        power -= *defense;
        if (power < 0)
            power = 0;
        destination->power = power;
    }
    SceneActor_SetContactReactionDirection(actor, source, destination);
    actor->field2E0 = source->kind;
    {
        u32 parameter = source->parameter34;
        u32 reaction = source->reactionFlags;
        u32 offset = 0x2D8;
        *(u32 *)((u32)actor + offset) = parameter;
        {
            u32 offset = 0x2DC;
            *(u32 *)((u32)actor + offset) = reaction;
        }
    }
    {
        u32 mask = 0x800;
        if (Test(gGameState->flags868, mask) || (ReadReactionFlags(0x40, source))) {
            destination->remaining = 0;
            destination->power = 0;
            actor->reactionSoundVariant = 0;
            actor->field2E6 = 1;
        }
    }
    actor->reactionSoundVariant = destination->remaining;
    PlayerStats_AddContactHundredths(source, destination, actor->fieldA0);
    {
        u32 mask = 0x2000;
        u32 offset = 0x2CC;
        u32 *field;
        field = (void *)((u32)actor + offset);
        *field |= mask;
    }
}
