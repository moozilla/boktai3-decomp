#include "contact_collider.h"

void Collider_SetAttackProperties(struct ContactCollider *collider, s32 power, s32 knockbackPower,
                                  s32 weakness, u32 attributes, s32 invincibilityFrames)
{
    collider->unknown3C = power;
    collider->responseParameter = knockbackPower;
    collider->statRewardFlags = weakness;
    collider->reactionFlags = attributes;
    collider->responseValue = invincibilityFrames;
}
