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
    return SceneActorPlacement_IsUnavailable(actor);
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
        actor->field2E6 = mode;
    }
    if ((u32)(actor->category - 18) <= 2 || actor->category == 23) {
        destination->power = source->power;
    } else if (ReadReactionFlags(0x40000, source)) {
        u8 *defense = &actor->defense;
        s32 power;
        power = source->power;
        power -= *defense;
        if (power < 0)
            power = 0;
        destination->power = power;
    }
    SceneActor_SetContactReactionDirection(actor, source, destination);
    actor->hitOwners = source->kind;
    {
        u32 parameter = source->parameter34;
        u32 reaction = source->reactionFlags;
        actor->hitAttributes = parameter;
        {
            actor->hitReactionFlags = reaction;
        }
    }
    {
        u32 mask = 0x800;
        if (SceneActorPlacement_TestMask(gGameState->flags868, mask) ||
            (ReadReactionFlags(0x40, source))) {
            destination->remaining = 0;
            destination->power = 0;
            actor->reactionSoundVariant = 0;
            actor->field2E6 = 1;
        }
    }
    actor->reactionSoundVariant = destination->remaining;
    PlayerStats_AddContactHundredths(source, destination, actor->contactReward);
    {
        u32 mask = 0x2000;
        u32 *field;
        field = &actor->behaviorFlags;
        *field |= mask;
    }
}
