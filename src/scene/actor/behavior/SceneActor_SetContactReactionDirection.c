#include "global.h"
#include "scene/actor/scene_actor_behavior.h"
s32 Math_Atan2(s32, s32);
void SceneActor_SetContactReactionDirection(struct RoomActor *actor,
                                            struct SceneActorBehaviorContactView *source,
                                            struct SceneActorBehaviorContactView *destination)
{
    struct ContactPosition difference;
    if (source->flags & 0x100) {
        actor->reactionDirection = source->direction;
    } else {
        struct ContactPosition *delta = &difference;
        struct ContactPosition *from = &source->position;
        struct ContactPosition *to = &destination->position;
        difference.x = to->x - from->x;
        delta->y = to->y - from->y;
        delta->z = to->z - from->z;
        if (!difference.x && !difference.z) {
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            actor->reactionDirection = ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
        } else {
            actor->reactionDirection = Math_Atan2(difference.x, difference.z);
        }
    }
}
