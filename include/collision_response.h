#ifndef GUARD_COLLISION_RESPONSE_H
#define GUARD_COLLISION_RESPONSE_H

#include "contact_collider.h"

void Collider_SetBodyProperties(void *collider, u16 power, u32 reactionFlags, u32 rewardFlags);
void Collision_CalculateDamageResponse(struct ContactCollider *attacker,
                                       struct ContactCollider *defender);

#endif
