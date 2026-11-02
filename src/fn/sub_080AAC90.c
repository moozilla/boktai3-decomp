#include "scene_actor_behavior.h"
#include "scene_actor_placement.h"
void Collision_CalculateDamageResponse(struct SceneActorBehaviorContactView *,
                                       struct SceneActorBehaviorContactView *);
void TentacleActor_EnterHitReaction(void *);
s32 Math_Atan2(s32, s32);
static inline u32 Unavailable(struct SceneActorBehaviorView *actor)
{
    return SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor);
}
struct TentacleContactSegment {
    u8 unknown0[0x108];
    u8 reactionDirection;
    u8 unknown109[0x124 - 0x109];
    struct SceneActorBehaviorView *target;
};
struct TentacleContactTarget {
    u8 unknown0[0x8EC];
    u32 closestDistance;
    void *closestSegment;
};

void TentacleActor_ReactToContact(struct SceneActorBehaviorContactView *source,
                                  struct SceneActorBehaviorContactView *destination, void *input)
{
    struct TentacleContactSegment *actor = input;
    struct TentacleContactSegment *self = actor;
    struct TentacleContactTarget *target = (void *)actor->target;
    if (!Unavailable((void *)target)) {
        struct ContactPosition difference;
        struct ContactPosition *delta = &difference;
        struct ContactPosition *right = (void *)((u8 *)destination + 0x24);
        struct ContactPosition *left = (void *)((u8 *)source + 0x24);
        const u16 *table;
        u32 distance;

        difference.x = (u16)left->x - (u16)right->x;
        delta->y = (u16)left->y - (u16)right->y;
        table = gRandomTable;
        delta->z = (u16)left->z - (u16)right->z;
        distance = difference.x * difference.x + difference.z * difference.z;
        if (distance < target->closestDistance) {
            Collision_CalculateDamageResponse(source, destination);
            destination->parameter44 = source->parameter44;
            destination->power = source->power;
            if (source->flags & 0x100) {
                self->reactionDirection = source->direction;
            } else if (!difference.x && !difference.z) {
                gRandomIndex = (gRandomIndex + 1) & 1023;
                self->reactionDirection = table[gRandomIndex];
            } else {
                self->reactionDirection = Math_Atan2(difference.x, difference.z);
            }
            target->closestDistance = distance;
            target->closestSegment = actor;
        }
        TentacleActor_EnterHitReaction(target);
    }
}
