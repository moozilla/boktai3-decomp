#include "global.h"

void Mummy_UpdateAttackCollider(void *);
void Mummy_UpdateMovementTimers(void *);
void Mummy_UpdateMovementCooldown(void *);
void Mummy_UpdateBurnDamage(void *);
void Mummy_UpdateTemporaryFlagTimer(void *);

s32 Mummy_UpdateBehavior(void *p)
{
    Mummy_UpdateAttackCollider(p);
    Mummy_UpdateMovementTimers(p);
    Mummy_UpdateMovementCooldown(p);
    Mummy_UpdateBurnDamage(p);
    Mummy_UpdateTemporaryFlagTimer(p);
    return 1;
}
