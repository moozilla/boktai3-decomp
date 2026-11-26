#include "bosses/vanargand/vanargand_damage_part.h"

void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, u32);

void Vanargand_OnPartContact(struct ContactCollider *source, struct ContactCollider *receiver,
                            struct VanargandDamagePart *part)
{
    u32 flags;
    Collision_CalculateDamageResponse(source, receiver);
    part->pendingDamage = receiver->cost;
    flags = source->reactionFlags;
    part->pendingFlags = flags;
    part->contactCooldown = source->responseValue;
    PlayerStats_AddContactHundredths(source, receiver, 10);
}
