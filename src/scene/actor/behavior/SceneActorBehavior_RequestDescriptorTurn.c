#include "engine/math/math_shift.h"
#include "scene/actor/scene_actor_behavior.h"
static inline s32 Magnitude(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
u8 SceneActorBehavior_RequestDescriptorTurn(struct RoomActor *actor,
                                            struct SceneActorBehaviorDestination *descriptor,
                                            u32 rangeX, u32 rangeZ)
{
    s16 dx = (u16)descriptor->destination.x - (u16)actor->body.position.x;
    s16 dz = (u16)descriptor->destination.z - (u16)actor->body.position.z;
    u32 targetQuadrant;
    u32 currentQuadrant;
    if ((u32)Magnitude(dx) <= rangeX && (u32)Magnitude(dz) <= rangeZ)
        goto failure;
    if (!dx && !dz)
        goto failure;
    targetQuadrant = ((u32)(Math_Atan2(dx, dz) + 32) & 255) >> 6;
    currentQuadrant = ((actor->body.direction + 32) & 255) >> 6;
    if (!Magnitude(targetQuadrant - currentQuadrant))
        goto failure;
    targetQuadrant = (u8)(targetQuadrant << 6);
    actor->nextDirection = (Math_ShiftTowardZero(targetQuadrant, 5) + 3) & 7;
    return 1;
failure:
    return 0;
}
