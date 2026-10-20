#include "global.h"

void sub_080D7F50(void *);
void Mummy_UpdateMovementTimers(void *);
void Mummy_UpdateMovementCooldown(void *);
void Mummy_UpdateBurnDamage(void *);
void Mummy_UpdateTemporaryFlagTimer(void *);

s32 Mummy_UpdateBehavior(void *p)
{
    sub_080D7F50(p);
    Mummy_UpdateMovementTimers(p);
    Mummy_UpdateMovementCooldown(p);
    Mummy_UpdateBurnDamage(p);
    Mummy_UpdateTemporaryFlagTimer(p);
    return 1;
}
