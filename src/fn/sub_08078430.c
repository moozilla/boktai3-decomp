// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
#define SceneActorBehavior_AccelerateTowardDescriptor sub_08078430
static inline s32 Magnitude(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline s32 ScaledComponent(s32 component, s32 inputSpeed)
{
    s32 product = component;
    register s32 speed asm("r6") = inputSpeed;
    product = (u32)product * (u32)speed;
    if (product >= 0)
        product >>= 16;
    else
        product = -((s32)(0u - (u32)product) >> 16);
    return product;
}
u8 SceneActorBehavior_AccelerateTowardDescriptor(
    struct SceneActorBehaviorView *actor, struct SceneActorBehaviorDestination *inputDescriptor,
    u32 inputRangeX, u32 inputRangeZ, s32 speed)
{
    struct SceneActorBehaviorDestination *descriptor = inputDescriptor;
    u32 rangeX = inputRangeX;
    register u32 rangeZ asm("r8") = inputRangeZ;
    register s32 actorX asm("r5");
    s32 actorZ;
    s32 dx;
    s32 dz;
    s32 x;
    s32 z;
    {
        s32 coordinate = descriptor->destination.x;
        s16 *position = &actor->position.x;
        actorX = *position;
        dx = coordinate - actorX;
        coordinate = descriptor->destination.z;
        position = (void *)((u32)position + 4);
        {
            register s32 value asm("r2") = *position;
            actorZ = value;
            dz = coordinate - value;
        }
    }
    {
        u32 mask = 0x1000;
        if (actor->flags2D4 & mask) {
            if (!(descriptor->flags & 1) && (s32)((u32)dx * dx + (u32)dz * dz) <= 0x4000)
                return 1;
        } else if ((u32)Magnitude(descriptor->position.x - actorX) <= rangeX &&
                   (u32)Magnitude(descriptor->position.z - actorZ) <= rangeZ)
            return 1;
    }
    if (dx || dz) {
        register s32 preservedSpeed asm("r6");
        actor->heading = Math_Atan2(dx, dz);
        {
            const s16 *lookup;
            s32 component;
            {
                register u8 *heading asm("r2") = &actor->heading;
                const s16 *table = gUnk_086149C4;
                component = table[(*heading + 64) & 255];
                preservedSpeed = speed;
                component = (u32)component * (u32)preservedSpeed;
                if (component >= 0)
                    component >>= 16;
                else
                    component = -((s32)(0u - (u32)component) >> 16);
                x = component;
                {
                    register const s16 *table asm("r0") = gUnk_086149C4;
                    lookup = (void *)((u32)(*heading * 2) + (u32)table);
                }
            }
            component = *lookup;
            z = ScaledComponent(component, speed);
        }
        {
            s16 *field = &actor->velocity.x;
            s32 old = *field;
            s32 magnitude = x;
            asm("" : "+r"(old) : "r"(preservedSpeed));
            if (x < 0)
                magnitude = -x;
            if (magnitude > 10)
                old += x;
            *field = old;
        }
        {
            s16 *field = &actor->velocity.z;
            s32 old = *field;
            s32 magnitude = z;
            if (z < 0)
                magnitude = -z;
            if (magnitude > 10)
                old += z;
            *field = old;
        }
    }
    return 0;
}
