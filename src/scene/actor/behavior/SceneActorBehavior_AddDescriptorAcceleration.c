#include "engine/math/math_shift.h"
#include "scene/actor/scene_actor_behavior.h"
/* SceneActorBehavior_AddDescriptorAcceleration: Steers toward the descriptor destination by adding speed-scaled acceleration and actor biases to velocity */
u8 SceneActorBehavior_AddDescriptorAcceleration(struct RoomActor *actor,
                                                struct SceneActorBehaviorDestination *descriptor,
                                                u32 rangeX, u32 rangeZ, s32 speed)
{
    s16 dx = descriptor->destination.x - actor->body.position.x;
    s16 dz = descriptor->destination.z - actor->body.position.z;
    s32 step;
    u8 heading;
    if ((u32)abs(descriptor->position.x - actor->body.position.x) <= rangeX &&
        (u32)abs(descriptor->position.z - actor->body.position.z) <= rangeZ)
        return 1;
    if (dx || dz) {
        heading = Math_Atan2(dx, dz);
        step = Math_ShiftTowardZero(gUnk_086149C4[(heading + 64) & 255] * speed, 16);
        actor->body.motion.x += actor->reactionStart.x + step;
        step = Math_ShiftTowardZero(gUnk_086149C4[heading] * speed, 16);
        actor->body.motion.z += actor->reactionStart.z + step;
        actor->body.direction = heading;
    }
    return 0;
}
