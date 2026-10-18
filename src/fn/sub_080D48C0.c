#include "global.h"

void sub_080D7F50(void *);
void Mummy_UpdateMovementTimers(void *);
void Mummy_UpdateMovementCooldown(void *);
void sub_080D812C(void *);
void Mummy_UpdateTemporaryFlagTimer(void *);

s32 Mummy_UpdateBehavior(void *p)
{
    sub_080D7F50(p);
    Mummy_UpdateMovementTimers(p);
    Mummy_UpdateMovementCooldown(p);
    sub_080D812C(p);
    Mummy_UpdateTemporaryFlagTimer(p);
    return 1;
}
