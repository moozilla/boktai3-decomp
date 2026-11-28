#include "global.h"
#include "bosses/vanargand/vanargand_damage_part.h"

s32 Vanargand_OnPartTrigger(struct ContactCollider *collider, s32 message, s32 damage)
{
    struct VanargandDamagePart *part = collider->owner;

    if (message == 0) {
        damage -= part->targetDefense;
        if (damage > 0)
            part->pendingDamage = damage;
        part->targetActive = 1;
    } else if (message == 1) {
        part->targetActive = 0;
    }
    return 1;
}
