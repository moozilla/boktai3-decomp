#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_actor_placement.h"
#include "monsters/centipede/centipede_body.h"
void Collision_CalculateDamageResponse(struct SceneActorBehaviorContactView *,
                                       struct SceneActorBehaviorContactView *);
s32 Math_Atan2(s32, s32);
void Centipede_ReactToContact(struct SceneActorBehaviorContactView *source,
                              struct SceneActorBehaviorContactView *destination, void *input)
{
    struct TentacleAnimationSegment *segment = input;
    struct TentacleAnimationSegment *self = segment;
    struct TentacleBody *body = segment->owner;
    if (!SceneActorPlacement_IsUnavailable(&body->base)) {
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
        if (distance < body->pendingImpulse) {
            Collision_CalculateDamageResponse(source, destination);
            destination->parameter44 = source->parameter44;
            destination->power = source->power;
            if (source->flags & 0x100) {
                self->impulseHeading = source->direction;
            } else if (!difference.x && !difference.z) {
                gRandomIndex = (gRandomIndex + 1) & 1023;
                self->impulseHeading = table[gRandomIndex];
            } else {
                self->impulseHeading = Math_Atan2(difference.x, difference.z);
            }
            body->pendingImpulse = distance;
            body->impulseSegment = segment;
        }
        Centipede_EnterHitReaction((void *)body);
    }
}
