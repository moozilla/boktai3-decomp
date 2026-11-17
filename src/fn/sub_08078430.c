#include "scene_actor_behavior.h"
static inline s32 Magnitude(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline s32 ScaleProduct(s32 product)
{
    if (product >= 0)
        product >>= 16;
    else
        product = -((s32)(0 - (u32)product) >> 16);
    return product;
}
u8 SceneActorBehavior_AccelerateTowardDescriptor(struct RoomActor *actor,
                                                 struct SceneActorBehaviorDestination *descriptor,
                                                 u32 rangeX, u32 rangeZ, s32 speed)
{
    s32 destination = descriptor->destination.x;
    struct ContactPosition *position = &actor->body.position;
    s32 actorX = position->x;
    s32 dx = destination - actorX;
    s32 actorZ;
    s32 dz;
    s32 x, z;
    u32 mask;
    destination = descriptor->destination.z;
    {
        s16 *field = &position->z;
        actorZ = *field;
    }
    dz = destination - actorZ;
    mask = 0x1000;
    if (actor->motionFlags & mask) {
        if (!(descriptor->flags & 1) && dx * dx + dz * dz <= 0x4000)
            return 1;
    } else if ((u32)Magnitude(descriptor->position.x - actorX) <= rangeX &&
               (u32)Magnitude(descriptor->position.z - actorZ) <= rangeZ)
        return 1;
    if (dx || dz) {
        actor->body.direction = Math_Atan2(dx, dz);
        x = ScaleProduct(gUnk_086149C4[(actor->body.direction + 64) & 255] * speed);
        z = ScaleProduct(gUnk_086149C4[actor->body.direction] * speed);
        {
            s16 *field = &actor->body.motion.x;
            s32 old = *field;
            if (Magnitude(x) > 10)
                old += x;
            *field = old;
        }
        {
            s16 *field = &actor->body.motion.z;
            s32 old = *field;
            if (Magnitude(z) > 10)
                old += z;
            *field = old;
        }
    }
    return 0;
}
