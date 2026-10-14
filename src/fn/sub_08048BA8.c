#include "tether_actor.h"
void sub_08068124(void *, void *, void *, u32, u32, u32);
static inline s32 scaleEmission(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void TetherActor_EmitRingEffects(struct TetherActor *actor)
{
    struct ContactPackedVector velocity, position, size;
    s32 index = 0;
    do {
        u32 angle = index * 32;
        velocity.x = scaleEmission(gUnk_086149C4[(angle + 64) & 255] * 16);
        velocity.y = 0;
        angle &= 255;
        velocity.z = scaleEmission(gUnk_086149C4[angle] * 16);
        position.x = actor->position.x + (u16)velocity.x * 8;
        position.y = actor->position.y;
        position.z = actor->position.z + (u16)velocity.z * 8;
        size.x = 0;
        size.y = 0;
        size.z = 0;
        sub_08068124(&position, &velocity, &size, 1, 3, 4);
        ++index;
    } while (index <= 7);
}
