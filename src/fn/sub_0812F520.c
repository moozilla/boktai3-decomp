#include "global.h"
#include "scene_actor_behavior.h"
s32 Math_Atan2(s32, s32);
void SceneActor_SetContactReactionDirection(struct SceneActorBehaviorView *actor,
                                            struct SceneActorBehaviorContactView *source,
                                            struct SceneActorBehaviorContactView *destination)
{
    struct ContactPosition difference;
    if (source->flags & 0x100) {
        actor->reactionDirection = source->direction;
    } else {
        struct ContactPosition *delta = &difference;
        struct ContactPosition *right = &source->position;
        struct ContactPosition *left = &destination->position;
        difference.x = (u16)destination->position.x - (u16)source->position.x;
        delta->y = (u16)left->y - (u16)right->y;
        delta->z = (u16)left->z - (u16)right->z;
        if (!difference.x && !difference.z) {
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            actor->reactionDirection = *(const u16 *)(gRandomIndex * 2 + (u32)gUnk_0203B400);
        } else {
            actor->reactionDirection = Math_Atan2(difference.x, difference.z);
        }
    }
}
