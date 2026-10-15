// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
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
    return -((s32)(0u - (u32)value) >> 5);
}
u8 SceneActorBehavior_RequestDescriptorTurn(struct SceneActorBehaviorView *actor,
                                            struct SceneActorBehaviorDestination *descriptor,
                                            u32 rangeX, u32 rangeZ)
{
    s16 dx = (u16)descriptor->destination.x - (u16)actor->position.x;
    s16 dz = (u16)descriptor->destination.z - (u16)actor->position.z;
    u32 targetQuadrant;
    u32 currentQuadrant;
    register s32 direction asm("r2");
    if ((u32)Magnitude(dx) <= rangeX && (u32)Magnitude(dz) <= rangeZ)
        goto failure;
    {
        s32 signedX = dx;
        if (!signedX && !dz)
            goto failure;
        targetQuadrant = ((u32)(Math_Atan2(signedX, dz) + 32) & 255) >> 6;
    }
    currentQuadrant = ((actor->heading + 32) & 255) >> 6;
    if (!Magnitude(targetQuadrant - currentQuadrant))
        goto failure;
    {
        register u32 shifted asm("r0") = targetQuadrant << 30;
        asm("" : "+r"(shifted));
        direction = shifted >> 24;
    }
    asm("" : : "r"(direction));
    actor->requestedDirection = (Divide32(direction) + 3) & 7;
    return 1;
failure:
    return 0;
}
