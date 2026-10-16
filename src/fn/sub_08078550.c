#include "scene_actor_behavior.h"
#define SceneActorBehavior_AddDescriptorAcceleration sub_08078550
static inline s32 Magnitude(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline s32 ScaledComponent(s32 component, s32 speed)
{
    s32 product = (u32)component * (u32)speed;
    if (product >= 0)
        return product >> 16;
    return -((s32)(0u - (u32)product) >> 16);
}
u8 SceneActorBehavior_AddDescriptorAcceleration(struct SceneActorBehaviorView *actor,
                                                struct SceneActorBehaviorDestination *descriptor,
                                                u32 rangeX, u32 rangeZ, s32 speed)
{
    s16 dx = (u16)descriptor->destination.x - (u16)actor->position.x;
    s16 dz = (u16)descriptor->destination.z - (u16)actor->position.z;
    s32 step;
    u8 heading;
    if ((u32)Magnitude(descriptor->position.x - actor->position.x) <= rangeX &&
        (u32)Magnitude(descriptor->position.z - actor->position.z) <= rangeZ)
        return 1;
    if (dx || dz) {
        heading = Math_Atan2(dx, dz);
        step = ScaledComponent(gUnk_086149C4[(heading + 64) & 255], speed);
        actor->velocity.x = (u16)actor->velocity.x + (u16)(actor->biasX310 + step);
        step = ScaledComponent(gUnk_086149C4[heading], speed);
        actor->velocity.z = (u16)actor->velocity.z + (u16)(actor->biasZ314 + step);
        actor->heading = heading;
    }
    return 0;
}
