#include "scene/collision/collision_response.h"
#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_actor_placement.h"
void SceneActor_SetContactReactionDirection(struct RoomActor *,
                                            struct ContactCollider *,
                                            struct ContactCollider *);
void PlayerStats_AddContactHundredths(struct ContactCollider *, void *, u32);
static inline u32 Unavailable(struct RoomActor *actor)
{
    return SceneActorPlacement_IsUnavailable(actor);
}
static inline u32 ReadReactionFlags(u32 inputMask, struct ContactCollider *source)
{
    u32 mask = inputMask;
    return source->reactionFlags & mask;
}
void SceneActorBehavior_ApplyContactReaction(struct ContactCollider *source,
                                             struct ContactCollider *destination,
                                             struct RoomActor *actor)
{
    struct ContactCollider *primary;
    u32 unavailable = Unavailable(actor);
    if (unavailable)
        return;
    primary = &actor->colliderA;
    if (actor->colliderA.responseValue) {
        if (actor->field2E6) {
            destination->cost = unavailable;
            destination->costFlags = 0;
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
    destination->responseValue = source->responseValue;
    {
        u32 mode = destination->costFlags;
        actor->field2E6 = mode;
    }
    if ((u32)(actor->category - 18) <= 2 || actor->category == 23) {
        destination->responseParameter = source->responseParameter;
    } else if (ReadReactionFlags(0x40000, source)) {
        u8 *defense = &actor->defense;
        s32 power;
        power = source->responseParameter;
        power -= *defense;
        if (power < 0)
            power = 0;
        destination->responseParameter = power;
    }
    SceneActor_SetContactReactionDirection(actor, source, destination);
    actor->hitOwners = source->contactFlags;
    {
        u32 parameter = source->statRewardFlags;
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
            destination->cost = 0;
            destination->responseParameter = 0;
            actor->reactionSoundVariant = 0;
            actor->field2E6 = 1;
        }
    }
    actor->reactionSoundVariant = destination->cost;
    PlayerStats_AddContactHundredths(source, destination, actor->contactReward);
    {
        u32 mask = 0x2000;
        u32 *field;
        field = &actor->behaviorFlags;
        *field |= mask;
    }
}
