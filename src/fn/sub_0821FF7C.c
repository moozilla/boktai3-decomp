#include "collision_response.h"

void Collider_SetBodyProperties(void *raw, u16 power, u32 reactionFlags, u32 rewardFlags)
{
    struct ContactCollider *collider = raw;
    collider->unknown3C = power;
    collider->reactionFlags = reactionFlags;
    collider->statRewardFlags = rewardFlags;
}
