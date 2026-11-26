#include "actors/scene_actor_behavior/scene_actor_behavior.h"
static inline s32 Magnitude(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline s32 Divide32(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((s32)(0 - (u32)value) >> 5);
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
    {
        s32 signedX = dx;
        if (!signedX && !dz)
            goto failure;
        targetQuadrant = ((u32)(Math_Atan2(signedX, dz) + 32) & 255) >> 6;
    }
    currentQuadrant = ((actor->body.direction + 32) & 255) >> 6;
    if (!Magnitude(targetQuadrant - currentQuadrant))
        goto failure;
    {
        u32 shifted = targetQuadrant << 30;
        targetQuadrant = shifted >> 24;
    }
    actor->nextDirection = (Divide32(targetQuadrant) + 3) & 7;
    return 1;
failure:
    return 0;
}
