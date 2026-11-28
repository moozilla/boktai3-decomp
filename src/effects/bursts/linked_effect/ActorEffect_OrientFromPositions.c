#include "effects/bursts/linked_effect.h"

void LinkedEffect_SetVariant(void *, u32);

void ActorEffect_OrientFromPositions(struct ContactCollider *collider,
                                     struct ContactCollider *other, struct LinkedEffect *effect)
{
    struct ContactPosition delta;
    s32 angle;
    struct ContactCollider *colliders;

    LinkedEffect_Subtract(&delta, &other->position, &collider->position);
    if (delta.x == 0 && delta.z == 0) {
        u32 index = (gRandomIndex + 1) & 0x3ff;

        gRandomIndex = index;
        angle = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
    } else {
        angle = Math_Atan2(delta.x, delta.z);
    }
    LinkedEffect_SetVariant(&effect->sprite.position, (u8)angle);
    LinkedEffect_SetVariant(&effect->sprite.position, (u8)angle);
    colliders = effect->colliders;
    colliders->flags |= 4;
    colliders++;
    colliders->flags |= 4;
    effect->state = 3;
    effect->entered = 1;
}
