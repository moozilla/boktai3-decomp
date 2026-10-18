#include "global.h"
void Bee_TickBehaviorCooldown(u8 *);
void SceneActorCollision_FireSecondaryAttack(u8 *);
void Bee_TickActionCooldown(u8 *);
u32 Bee_UpdateTimersAndContacts(u8 *p)
{
    Bee_TickBehaviorCooldown(p);
    SceneActorCollision_FireSecondaryAttack(p);
    Bee_TickActionCooldown(p);
    return 1;
}
