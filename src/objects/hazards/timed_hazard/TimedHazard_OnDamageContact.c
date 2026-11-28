#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/timed_hazard.h"
#include "objects/hazards/falling_objects.h"

void TimedHazard_OnDamageContact(struct FallingHit *attack, struct FallingHit *target,
                                 struct TimedHazard *hazard)
{
    u32 damage = target->damage;
    u32 mask = 8;
    if (attack->flags & mask)
        damage *= 2;
    hazard->health -= damage;
    if (hazard->health < 0)
        hazard->health = 0;
}
